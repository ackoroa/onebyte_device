#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>

#define MAJOR_NUMBER 61

/* forward declaration */
int onebyte_open(struct inode *inode, struct file *filep);
int onebyte_release(struct inode *inode, struct file *filep);
ssize_t onebyte_read(struct file *filep, char *buf, size_t count, loff_t *f_pos);
ssize_t onebyte_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos);
static void onebyte_exit(void);

/* definition of file_operation structure */
struct file_operations onebyte_fops = {
  read: onebyte_read,
  write: onebyte_write,
  open: onebyte_open,
  release: onebyte_release
};

char* onebyte_data = NULL;
int onebyte_size = 0;

int onebyte_open(struct inode *inode, struct file *filep) {
  return 0; // always successful
}

int onebyte_release(struct inode *inode, struct file *filep) {
  return 0; // always successful
}

ssize_t onebyte_read(struct file *filep, char *buf, size_t count, loff_t *f_pos) {
  printk(KERN_ALERT "Reading from onebyte: %c\n", *onebyte_data);
  
  if (onebyte_size) {
    *buf = *onebyte_data;
    onebyte_size = 0;
    return 1;
  }

  return 0;
}

ssize_t onebyte_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos) {
  if (count <= 0) {
    printk(KERN_EMERG "write error: Empty Input.\n");
    return count;
  }

  printk(KERN_ALERT "Writing to onebyte: %c\n", *buf);
  *onebyte_data = buf[0];
  onebyte_size = 1;

  if (count > 1) {
    printk(KERN_ALERT "%.*s is longer than one byte", (int) count, buf); 
    return -ENOSPC;
  }

  return count;
}

static int onebyte_init(void) {
  int result;

  // register the device
  result = register_chrdev(MAJOR_NUMBER, "onebyte", &onebyte_fops);
  if (result < 0) {
    return result;
  }

  // allocate one byte of memory for storage
  // kmalloc is just like malloc, the second parameter
  // the type of memory to be allocated.
  // To release the memory allocated by kmalloc, use kfree.
  onebyte_data = kmalloc(sizeof(char), GFP_KERNEL);
  if (!onebyte_data) {
    onebyte_exit();
    // cannot allocate memory
    // return no memory error, negative signify a failure
    return -ENOMEM;
  }

  // initialize the value to be X
  *onebyte_data = 'X';
  onebyte_size = 1;
  printk(KERN_ALERT "This is a onebyte device module\n");
  return 0;
}

static void onebyte_exit(void) {
  // if the pointer is pointing to something
  if (onebyte_data) {
    // free the memory and assign the pointer to NULL
    kfree(onebyte_data);
    onebyte_data = NULL;
  }

  // unregister the device
  unregister_chrdev(MAJOR_NUMBER, "onebyte");
  printk(KERN_ALERT "Onebyte device module is unloaded\n");
}

MODULE_LICENSE("GPL");
module_init(onebyte_init);
module_exit(onebyte_exit);
