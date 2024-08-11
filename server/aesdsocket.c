#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stddef.h>
#include <arpa/inet.h>
#include <signal.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT "9000"
#define OUTPUT_FILE "/var/tmp/aesdsocketdata"
#define MAX_BUFFER 100

int output = -1;
struct addrinfo *res;
int fd;
int cli_fd;
char *send_buff;

void pr_err(char *msg, char ret);
static void signal_handler(int signum);
char find_char(char *src, char expect, unsigned char size);

int main(int argc, char **argv)
{
	struct addrinfo hint;
	struct sockaddr client;
	socklen_t cli_addr_len;
	int err;
	char ip[INET_ADDRSTRLEN];	
	ssize_t size;
	ssize_t total_size = 0;
	ssize_t ret;
	struct sigaction sig;
	char is_deamons = 0;
	pid_t pid;
	int opt = 1;
	ssize_t file_size = 0;
	char buffer[MAX_BUFFER];
	char end_line = -1;

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

	while (1)
	{
		cli_addr_len = sizeof(struct sockaddr);
		cli_fd = accept(fd, &client, &cli_addr_len);
		if (cli_fd == -1)
			return -1;

		inet_ntop(AF_INET, &(((struct sockaddr_in*)(&client))->sin_addr), ip, sizeof(ip));
		syslog(LOG_USER, "Accepted connection from %s\r\n", ip);

		if (output == -1)
			output = open(OUTPUT_FILE, O_RDWR|O_CREAT|O_APPEND, 0666);
		if (output == -1)
			return -1;
		while ((size = recv(cli_fd, buffer, MAX_BUFFER, 0)) > 0)
		{
			total_size += size;
			ret = write(output, buffer, size);
			if (ret < size)
				pr_err("Write to output fail\r\n", -1);

			end_line = find_char(buffer, '\n', MAX_BUFFER);
			if (end_line != -1) {
				memset(buffer, 0, MAX_BUFFER);
				if ((file_size = lseek(output, 0, SEEK_END)) == -1)
					pr_err("lseek fail\r\n", -1);
				send_buff = malloc(file_size);
				
				if (send_buff == NULL) {
					printf("Cannot allocate send buffer\r\n");
					exit(-1);
				}
				if (lseek(output, 0, SEEK_SET) == -1) {
					printf("lseek fail\r\n");
					free(send_buff);
					exit(-1);
				}
				ret = read(output, send_buff, file_size);
				if (ret < 0) {
					printf("Read fail\r\n");
					free(send_buff);
					exit(-1);
				}
				send(cli_fd, send_buff, file_size, 0);
				free(send_buff);

				close(cli_fd);
				break;
			}
		}
		syslog(LOG_USER, "Closed connection from %s\r\n", ip);
	}

	return 0;
}

static void signal_handler(int signum)
{
	if (signum == SIGINT || signum == SIGTERM) {
		syslog(LOG_USER, "Caught signal, exiting\r\n");
		freeaddrinfo(res);
		closelog();
		close(fd);
		close(cli_fd);
		close(output);
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

char find_char(char *src, char expect, unsigned char size)
{
	unsigned char i = 0;

	for (i=0; i<size; i++)
	{
		if (src[i] == expect)
			return 0;
	}
	return -1;
}