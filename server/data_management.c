#include <string.h>
#include <stdlib.h>
#include "data_management.h"

typedef struct slist_data_s slist_data_t;
static SLIST_HEAD(slisthead, slist_data_s) head;

void data_init(void)
{
    SLIST_INIT(&head);

}

struct slist_data_s* get_head(void)
{
	struct slist_data_s *ret;

	ret = SLIST_FIRST(&head);
	return ret;
}

char data_append(int cli_fd)
{
    slist_data_t *datap = NULL;

    datap = malloc(sizeof(slist_data_t));
    if (datap == NULL) 
	    return 0xFF;

    datap->cli_fd = cli_fd;
    datap->tid = 0;
    datap->success = 0;

    SLIST_INSERT_HEAD(&head, datap, entries);
    return 0;
}

struct slist_data_s* get_success(void)
{
    slist_data_t *datap;

    SLIST_FOREACH(datap, &head, entries) {
        if ((datap->success == 1) || (datap->success == 2)) {
            SLIST_REMOVE(&head, datap, slist_data_s, entries);
            return datap;
	    }
    }
    return NULL;
}
