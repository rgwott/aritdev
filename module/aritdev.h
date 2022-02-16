/**
 * \file aritdev.h
 *
 * \date 2022/02/15
 * \author Rodolfo Wottrich
 * \version 1.0
 *
 * \brief Declarations related to aritdev.c.
 *
 */


#ifndef ARITDEV_H_
#define ARITDEV_H_

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/fs.h>
#include <linux/module.h>

#define DEVICE_NAME  "aritdev"    // Dev name as it appears in /proc/devices

static int major_number;
static bool opened = false;
static uint8_t *response;
static int64_t result;
static bool result_available = false;

static struct class *cls;

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations chardev_fops = {
	.owner = THIS_MODULE,
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

#endif // ARITDEV_H_
