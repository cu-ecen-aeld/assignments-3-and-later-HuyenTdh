#ifndef _AESDSOCKET_H_
#define _AESDSOCKET_H_

#include <time.h>

void pr_err(char *msg, char ret);
unsigned char find_char(char *src, char expect, unsigned char size);
void print_arr(char *arr, int size);
/* char timer_init(timer_t *tim_id, unsigned int time, void *thread_param); */

#endif /* _AESDSOCKET_H_ */