/**
 * @file aesd-circular-buffer.c
 * @brief Functions and data related to a circular buffer imlementation
 *
 * @author Dan Walkes
 * @date 2020-03-01
 * @copyright Copyright (c) 2020
 *
 */

#ifdef __KERNEL__
#include <linux/string.h>
#include <linux/slab.h>
#else
#include <string.h>
#endif

#include "aesd-circular-buffer.h"

/**
 * @param buffer the buffer to get total memory.
 * @return total memory.
 */
size_t aesd_circular_buffer_total_size(struct aesd_circular_buffer *buffer)
{
    size_t size = 0;
    uint8_t i = 0;

    for (i = 0; i < (buffer->in_offs - buffer->out_offs); i++) {
        size += buffer->entry[buffer->out_offs].size;
    }

    return size;
}

/* size_t aesd_circular_buffer_add_data(struct aesd_circular_buffer *buffer, char *data, int data_size)
{
    int tmp;
    if (buffer->entry[buffer->in_offs].buffptr == NULL) {
        buffer->entry[buffer->in_offs].buffptr = kmalloc(data_size, GFP_KERNEL);
	if (buffer->entry[buffer->in_offs].buffptr == NULL)
	    return -ENOMEM;
	buffer->entry[buffer->in_offs].size = data_size;
	tmp = 0;
    }
    else {
        buffer->entry[buffer->in_offs].buffptr = krealloc(data_size + buffer->entry[buffer->in_offs].size,\
		       	GFP_KERNEL);
	if (buffer->entry[buffer->in_offs].buffptr == NULL)
            return -ENOMEM;
	tmp = buffer->entry[buffer->in_offs].size;
        buffer->entry[buffer->in_offs].size += data_size;
    }
    memcpy(&buffer->entry[buffer->in_offs].buffptr[tmp], data, data_size);

    return data_size;
} */

/**
 * @param buffer the buffer to search for corresponding offset.  Any necessary locking must be performed by caller.
 * @param char_offset the position to search for in the buffer list, describing the zero referenced
 *      character index if all buffer strings were concatenated end to end
 * @param entry_offset_byte_rtn is a pointer specifying a location to store the byte of the returned aesd_buffer_entry
 *      buffptr member corresponding to char_offset.  This value is only set when a matching char_offset is found
 *      in aesd_buffer.
 * @return the struct aesd_buffer_entry structure representing the position described by char_offset, or
 * NULL if this position is not available in the buffer (not enough data is written).
 */
struct aesd_buffer_entry *aesd_circular_buffer_find_entry_offset_for_fpos(struct aesd_circular_buffer *buffer,
            size_t char_offset, size_t *entry_offset_byte_rtn )
{
    /**
    * TODO: implement per description
    */
    size_t size = 0;
    size_t offset = char_offset;
    size_t i = 0;
    size_t cur_entry = 0;

    if (buffer->in_offs < buffer->out_offs)
	    size = buffer->in_offs + AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED - buffer->out_offs;
    else if (buffer->full)
	    size = AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED;
    else
	    size = buffer->in_offs;

    for (i = 0; i < size; i++) {
	    if ((i + buffer->out_offs) >= AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED)
		    cur_entry = (i + buffer->out_offs) % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED;
	    else
		    cur_entry = i + buffer->out_offs;

	    if (offset > (buffer->entry[cur_entry].size - 1))
		    offset -= buffer->entry[cur_entry].size;
	    else {
		    *entry_offset_byte_rtn = offset;
		    return &(buffer->entry[cur_entry]);
	    }
    }

    return NULL;
}

/**
* Adds entry @param add_entry to @param buffer in the location specified in buffer->in_offs.
* If the buffer was already full, overwrites the oldest entry and advances buffer->out_offs to the
* new start location.
* Any necessary locking must be handled by the caller
* Any memory referenced in @param add_entry must be allocated by and/or must have a lifetime managed by the caller.
*/
void aesd_circular_buffer_add_entry(struct aesd_circular_buffer *buffer, const struct aesd_buffer_entry *add_entry)
{
    /**
    * TODO: implement per description
    */
    if (buffer->full == true)
        buffer->out_offs = (buffer->out_offs + 1) % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED;

    buffer->entry[buffer->in_offs] = *add_entry;
    buffer->in_offs = (buffer->in_offs + 1) % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED;
    if (buffer->in_offs == buffer->out_offs)
        buffer->full  = true;
    else
        buffer->full = false;
}

/**
* Initializes the circular buffer described by @param buffer to an empty struct
*/
void aesd_circular_buffer_init(struct aesd_circular_buffer *buffer)
{
    memset(buffer,0,sizeof(struct aesd_circular_buffer));
}
