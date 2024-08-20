#ifndef _DATA_MANAGEMENT_H_
#define _DATA_MANAGEMENT_H_

#include <pthread.h>
#include <sys/queue.h>

struct slist_data_s {
    int cli_fd;
    pthread_t tid;
    unsigned char success;
    SLIST_ENTRY(slist_data_s) entries;
};

void data_init(void);
struct slist_data_s* get_head(void);
char data_append(int cli_fd);
struct slist_data_s* get_success(void);

#endif /* _DATA_MANAGEMENT_H_ */
