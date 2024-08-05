#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void err_log(const char *msg);

int main(int argc, char **argv)
{
    int fd = -1;
    ssize_t res = -1;

    if (argc < 3)
        err_log("Wrong format.");

    openlog(NULL,0,LOG_USER);
    fd = open(argv[1],O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IWOTH|S_IROTH);
    if (fd == -1)
        err_log("Open fail.");
    
    res = write(fd,argv[2],strlen(argv[2]));
    if (res < 0)
        err_log("Write fail.");

    syslog(LOG_DEBUG, "Writing %s to %s", argv[2], argv[1]);
    closelog();
    exit(0);
}

void err_log(const char *msg)
{
    syslog(LOG_ERR,"%s",msg);
    closelog();
    exit(1);
}