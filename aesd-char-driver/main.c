/**
 * @file aesdchar.c
 * @brief Functions and data related to the AESD char driver implementation
 *
 * Based on the implementation of the "scull" device driver, found in
 * Linux Device Drivers example code.
 *
 * @author Dan Walkes
 * @date 2019-10-22
 * @copyright Copyright (c) 2019
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/fs.h> // file_operations

#include <linux/mutex.h>
#include <linux/slab.h>

#include "aesdchar.h"
int aesd_major =   0; // use dynamic major
int aesd_minor =   0;

MODULE_AUTHOR("Huyen Do Van"); /** TODO: fill in your name **/
MODULE_LICENSE("Dual BSD/GPL");

static char *buffptr;
static size_t size;
struct aesd_dev aesd_device;

int aesd_open(struct inode *inode, struct file *filp)
{
    struct aesd_dev *aesd_device;

    PDEBUG("open");
    /**
     * TODO: handle open
     */
    aesd_device = container_of(inode->i_cdev, struct aesd_dev, cdev);
    filp->private_data = (void*)aesd_device;
    return 0;
}

int aesd_release(struct inode *inode, struct file *filp)
{
    PDEBUG("release");
    /**
     * TODO: handle release
     */
    return 0;
}

ssize_t aesd_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
    ssize_t retval = 0;
    struct aesd_dev *aesd_device = filp->private_data;
    struct aesd_buffer_entry *entry;
    char *tmp;
    size_t entry_offset = 0;
    size_t total_size = 0;
    size_t i;

    PDEBUG("read %zu bytes with offset %lld",count,*f_pos);
    /**
     * TODO: handle read
     */
    mutex_lock(&aesd_device->lock);
    tmp = kmalloc(count, GFP_KERNEL);
    if (tmp == NULL) {
        mutex_unlock(&aesd_device->lock);
        return -ENOMEM;
    }
    for (i = 0; i < count; i++) {
        entry = aesd_circular_buffer_find_entry_offset_for_fpos(&aesd_device->buffer, *f_pos, &entry_offset);
        if (entry == NULL) {
            break;
        }
        tmp[i] = entry->buffptr[entry_offset];
        *f_pos += 1;
    }
    if (copy_to_user(buf, tmp, i))
        retval = -EFAULT;
    else
        retval = i;
    mutex_unlock(&aesd_device->lock);
    kfree(tmp);

    return retval;
}

ssize_t aesd_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
    ssize_t retval = -ENOMEM;
    struct aesd_dev *aesd_device = filp->private_data;
    struct aesd_buffer_entry entry;
    char *tmp = NULL;
    int i = 0;

    PDEBUG("write %zu bytes with offset %lld",count,*f_pos);
    /**
     * TODO: handle write
     */
    tmp = kmalloc(count, GFP_KERNEL);
    if (tmp == NULL)
        return retval;
    if (copy_from_user(tmp, buf, count)) {
        kfree(tmp);
        return -EFAULT;
    }
    retval = count;

    mutex_lock(&aesd_device->lock);
    for (i = 0; i < retval; i++) {
        buffptr = krealloc(buffptr, size + 1, GFP_KERNEL);
        if (buffptr == NULL) {
            PDEBUG("Cannot allocate");
            kfree(tmp);
            return -ENOMEM;
        }
        buffptr[size] = tmp[i];
        size += 1;
        if (tmp[i] == '\n') {
            entry.buffptr = buffptr;
            entry.size = size;
            *f_pos += size;
            if (aesd_device->buffer.full)
                kfree(aesd_device->buffer.entry[aesd_device->buffer.in_offs].buffptr);
            aesd_circular_buffer_add_entry(&aesd_device->buffer, &entry);
            buffptr = NULL;
            size = 0;
        }
    }
    mutex_unlock(&aesd_device->lock);
    kfree(tmp);
    return retval;
}
struct file_operations aesd_fops = {
    .owner =    THIS_MODULE,
    .read =     aesd_read,
    .write =    aesd_write,
    .open =     aesd_open,
    .release =  aesd_release,
};

static int aesd_setup_cdev(struct aesd_dev *dev)
{
    int err, devno = MKDEV(aesd_major, aesd_minor);

    cdev_init(&dev->cdev, &aesd_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &aesd_fops;
    err = cdev_add (&dev->cdev, devno, 1);
    if (err) {
        printk(KERN_ERR "Error %d adding aesd cdev", err);
    }
    return err;
}

/* struct class *aesd_class;
struct device *aesd_d; */

int aesd_init_module(void)
{
    dev_t dev = 0;
    int result;
    result = alloc_chrdev_region(&dev, aesd_minor, 1,
            "aesdchar");
    aesd_major = MAJOR(dev);
    if (result < 0) {
        printk(KERN_WARNING "Can't get major %d\n", aesd_major);
        return result;
    }
    memset(&aesd_device,0,sizeof(struct aesd_dev));

    /**
     * TODO: initialize the AESD specific portion of the device
     */
    mutex_init(&aesd_device.lock);

    result = aesd_setup_cdev(&aesd_device);

    if( result ) {
        unregister_chrdev_region(dev, 1);
        return result;
    }
/*     aesd_class = class_create(THIS_MODULE, "aesd_class");
    if (IS_ERR(aesd_class)){
        result = PTR_ERR(aesd_class);
        goto class_error;
    }
    aesd_d = device_create(aesd_class, NULL, dev, NULL, "%s", "aesdchar");
    if (IS_ERR(aesd_d)) {
        result = PTR_ERR(aesd_d);
        goto device_error;
    } */
    return result;

/* device_error:
    class_destroy(aesd_class);
class_error:
    unregister_chrdev_region(dev, 1);
    return result; */
}

void aesd_cleanup_module(void)
{
    dev_t devno = MKDEV(aesd_major, aesd_minor);
    int i = 0;

    for (i = 0; i < AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED; i++) {
        if (aesd_device.buffer.entry[i].buffptr != NULL)
            kfree(aesd_device.buffer.entry[i].buffptr);
    }
/*     device_destroy(aesd_class, devno);
    class_destroy(aesd_class); */
    cdev_del(&aesd_device.cdev);

    /**
     * TODO: cleanup AESD specific poritions here as necessary
     */
    mutex_destroy(&aesd_device.lock);

    unregister_chrdev_region(devno, 1);
}

module_init(aesd_init_module);
module_exit(aesd_cleanup_module);
