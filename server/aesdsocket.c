#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stddef.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/queue.h>

#include "data_management.h"
#include "aesdsocket.h"
#include "aesd_ioctl.h"

#define PORT "9000"

#ifdef USE_AESD_CHAR_DEVICE
#define OUTPUT_FILE "/dev/aesdchar"
#else
#define OUTPUT_FILE "/var/tmp/aesdsocketdata"
#endif
#define MAX_BUFFER 100

struct {
    int fd;
    pthread_mutex_t *mutex;
}shared_data;

struct addrinfo *res;
int fd;
int cli_fd;

static void signal_handler(int signum);
void* thread_func(void *param);
/* static void timer_func(union sigval thread_param); */

int main(int argc, char **argv)
{
    struct addrinfo hint;
    struct sockaddr client;
    socklen_t cli_addr_len;
    int err;
    char ip[INET_ADDRSTRLEN];    
    struct sigaction sig;
    char is_deamons = 0;
    pid_t pid;
    int opt = 1;
    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
    struct slist_data_s ret;
    struct slist_data_s *is_success;
    /* timer_t tim_id; */

    cli_addr_len = sizeof(struct sockaddr);
    shared_data.fd = open(OUTPUT_FILE, O_RDWR|O_CREAT|O_APPEND, 0666);
    if (shared_data.fd == -1)
        return -1;
    shared_data.mutex = &mtx;

    memset((char *)&sig, 0, sizeof(struct sigaction));
    sig.sa_handler = signal_handler;
    if (sigaction(SIGTERM, &sig, NULL) != 0) {
        printf("sigaction");
        exit(1);
        }
    if (sigaction(SIGINT, &sig, NULL) != 0) {
        printf("sigaction");
        exit(1);
    }

    if (argc == 2) {
        if (strcmp("-d", argv[1]) == 0)
            is_deamons = 1;
    }
    if (is_deamons) {
        pid = fork();
        switch (pid)
        {
        case 0:
            printf("Start deamons\r\n");
            break;
        case -1:
            printf("fork fail\r\n");
            exit(-1);
            break;

        default:
            printf("Parent exit. Child's id: %d\r\n", pid);
            exit(0);
            break;
        }
    }

    openlog(NULL, 0, LOG_USER);
    memset((char*)&hint, 0x00, sizeof(hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        pr_err("Cannot open socket\r\n", -1);
    }
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        pr_err("Cannot set socket option\r\n", -1);
    }
    err = getaddrinfo(NULL, PORT, &hint, &res);
    if (err)
        pr_err((char *)gai_strerror(err), -1);
    if (bind(fd, res->ai_addr, sizeof(struct sockaddr)) == -1)
        pr_err(strerror(errno), -1);

    if (listen(fd, 10) != 0)
        return -1;
    data_init();

/*     if (timer_init(&tim_id, 10, NULL) == 0xFF)
        return -1; */

    while (1)
    {
        cli_fd = accept(fd, &client, &cli_addr_len);
        if (cli_fd == -1)
            return -1;

        inet_ntop(AF_INET, &(((struct sockaddr_in*)(&client))->sin_addr), ip, sizeof(ip));
        syslog(LOG_USER, "Accepted connection from %s\r\n", ip);
        data_append(cli_fd);
        if (pthread_create(&(((struct slist_data_s*)get_head())->tid), NULL, thread_func, (void*)get_head()) != 0) {
            pr_err("Cannot create thread\r\n", -1);
        }

        is_success = get_success();
        if (is_success == NULL)
            continue;
        if (is_success->success != 0) {
            if (pthread_join(is_success->tid, (void*)&ret) != 0)
                pr_err("Cannot join thread\r\n", -1);
            close(is_success->cli_fd);
            free(is_success);
            syslog(LOG_USER, "Closed connection from %s\r\n", ip);
        }
    }
    return 0;
}

void* thread_func(void *param)
{
    struct slist_data_s *data = (struct slist_data_s*)param;
    ssize_t size;
    ssize_t ret;
    unsigned char end_line = 0xFF;
    char *buffer;
    ssize_t total = 0;
    ssize_t max = MAX_BUFFER;
    off_t file_size;
    struct aesd_seekto tmp;

    data->tid = pthread_self();
    buffer = malloc(MAX_BUFFER);
    if (buffer == NULL)
        return param;
    do {
        memset(buffer, 0, max);
        size = recv(data->cli_fd, &buffer[total], MAX_BUFFER, 0);
        if (size > 0) {
            total += size;
            end_line = find_char(buffer, '\n', MAX_BUFFER);
            if (end_line != 0xFF)
                break;
            }
        if (total + MAX_BUFFER > max) {
            max += MAX_BUFFER;
            buffer = realloc(buffer, max);
        }
    }while(end_line == 0xFF);

    pthread_mutex_lock(shared_data.mutex);
    if (sscanf(buffer, "AESDCHAR_IOCSEEKTO:%u,%u", &tmp.write_cmd, &tmp.write_cmd_offset)) {
        file_size = lseek(shared_data.fd, 0, SEEK_END);
        if (ioctl(shared_data.fd, AESDCHAR_IOCSEEKTO , &tmp) != 0) {
	    printf("%s\r\n", strerror(errno));
            syslog(LOG_USER, "ioctl fail\r\n");
            pthread_mutex_unlock(shared_data.mutex);
            data->success = 2;
	    free(buffer);
            return param;
        }
	file_size -= lseek(shared_data.fd, 0, SEEK_CUR);
        buffer = realloc(buffer, file_size);
        memset(buffer, 0, file_size);
        ret = read(shared_data.fd, buffer, file_size);
        if (ret < 0) {
            printf("Read fail\r\n");
            pthread_mutex_unlock(shared_data.mutex);
            data->success = 2;
            free(buffer);
            return param;
        }
    }
    else {
        ret = write(shared_data.fd, buffer, total);
        if (ret < total) {
            printf("Write to output fail\r\n");
            syslog(LOG_USER, "Write to output fail\r\n");
            pthread_mutex_unlock(shared_data.mutex);
            data->success = 2;
	    free(buffer);
            return param;
        }
        file_size = lseek(shared_data.fd, 0, SEEK_END);

        if (lseek(shared_data.fd, 0, SEEK_SET) == -1) {
            printf("lseek fail\r\n");
            pthread_mutex_unlock(shared_data.mutex);
            data->success = 2;
	    free(buffer);
            return param;
        }

        buffer = realloc(buffer, file_size);
        memset(buffer, 0, file_size);
        ret = read(shared_data.fd, buffer, file_size);
        if (ret < 0) {
            printf("Read fail\r\n");
            pthread_mutex_unlock(shared_data.mutex);
            data->success = 2;
	    free(buffer);
            return param;
        }
    }
    pthread_mutex_unlock(shared_data.mutex);
    send(data->cli_fd, buffer, ret, 0);
    data->success = 1;
    free(buffer);
    return param;
}

static void signal_handler(int signum)
{
    if (signum == SIGINT || signum == SIGTERM) {
        syslog(LOG_USER, "Caught signal, exiting\r\n");
        freeaddrinfo(res);
        closelog();
        close(fd);
        remove(OUTPUT_FILE);
        exit(0);
    }
}

void pr_err(char *msg, char ret)
{
    printf("%s", msg);
    syslog(LOG_ERR, "%s", msg);
    exit(ret);
}

unsigned char find_char(char *src, char expect, unsigned char size)
{
    unsigned char i = 0;

    for (i=0; i<size; i++)
    {
        if (src[i] == expect)
            return i;
    }
    return 0xFF;
}

void print_arr(char *arr, int size)
{
    int i = 0;

    for (i=0; i<size; i++) {
        printf("%c",arr[i]);
    }
}

/* char timer_init(timer_t *tim_id, unsigned int time, void *thread_param)
{
    struct sigevent ev;
    struct itimerspec interval_tim;

    if (time == 0)
        return 0xFF;

    memset(&ev, 0, sizeof(struct sigevent));
    ev.sigev_notify = SIGEV_THREAD;
    ev.sigev_notify_function = timer_func;
    ev.sigev_value.sival_ptr = thread_param;
    if (timer_create(CLOCK_MONOTONIC, &ev, tim_id) == -1)
        return 0xFF;

    memset(&interval_tim, 0, sizeof(struct itimerspec));
    interval_tim.it_value.tv_sec = (time_t)time;
    interval_tim.it_value.tv_nsec = 0;
    interval_tim.it_interval.tv_sec = (time_t)time;
    interval_tim.it_interval.tv_nsec = 0;
    if (timer_settime(*tim_id, 0, &interval_tim, NULL) == -1)
        return 0xFF;

    return 0;
}

void timer_func(union sigval thread_param)
{
    time_t t;
    struct tm *ts;
    char stime[100];
    char fmt[125];
    char *time_fmt = "%a, %d %b %Y %T %z";
    
    t = time(NULL);
    if (t == -1)
        return;
    ts = localtime(&t);
    if (ts == NULL)
        return;
    if (strftime(stime, sizeof(stime), time_fmt, ts) == 0)
        return;
    sprintf(fmt, "timestamp:%s\r\n", stime);
    pthread_mutex_lock(shared_data.mutex);
    write(shared_data.fd, fmt, strlen(fmt));
    pthread_mutex_unlock(shared_data.mutex);

    return;
}
 */
