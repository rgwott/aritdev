/**
 * \file aritdev.c
 *
 * \date 2022/02/15
 * \author Rodolfo Wottrich
 * \version 1.0
 *
 * \brief A Linux module that creates a character device to provide basic
 *        arithmetic operations.
 *
 */


// Module header files
#include "aritdev.h"

// Linux header files
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/poll.h>

// Module initialization (upon module loading)
int init_module(void)
{
	major_number = register_chrdev(0, DEVICE_NAME, &chardev_fops);

	if (major_number < 0) {
		pr_alert("Registering chardev failed with errno %d\n", major_number);
		return major_number;
	}

	cls = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(cls, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);

	pr_info("Device created on /dev/%s\n", DEVICE_NAME);

	return 0;
}

// Module cleanup (upon module unloading)
void cleanup_module(void)
{
	device_destroy(cls, MKDEV(major_number, 0));
	class_destroy(cls);
	unregister_chrdev(major_number, DEVICE_NAME);
}

// Device file descriptor opened
static int device_open(struct inode *inode, struct file *file)
{
	if (opened)
		return -EBUSY;

	opened = true;

	pr_info("Device opened\n");

	return 0;
}

// Device file descriptor closed
static int device_release(struct inode *inode, struct file *file)
{
	opened = false;

	return 0;
}

// Device's response is read, if available
static ssize_t device_read(struct file *filp, char *buf, size_t len,
                           loff_t * offset)
{
	int bytes_read = 0;
	int i;

	if (!result_available)
		return 0;

	response = (uint8_t *)&result;

	for (i = 0; i < sizeof(response); i++) {
		if (i >= len)
			break;

		// Copy result to userspace
		put_user(*(response++), buf++);

		bytes_read++;
	}

	result_available = false;

	return bytes_read;
}

// Device is sent a request
static ssize_t device_write(struct file *filp, const char *buf, size_t len,
                            loff_t * off)
{
	int i;
	int64_t operand1;
	int64_t operand2;
	char op;

	// Message must be exactly 17 bytes long (operation + 2 operands)
	if (len != sizeof(uint8_t) + (sizeof(int64_t) << 1) || buf == NULL) {
		return -EINVAL;
	}

	// Copy variables from userspace
	if (copy_from_user(&op, buf, sizeof(op)) != 0) {
		return -EINVAL;
	}
	if (copy_from_user(&operand1, &buf[1 + i], sizeof(operand1)) != 0) {
		return -EINVAL;
	}
	if (copy_from_user(&operand2, &buf[9 + i], sizeof(operand2)) != 0) {
		return -EINVAL;
	}

	// Perform operation
	switch (op) {
	case '+':
		result = operand1 + operand2;
		break;
	case '-':
		result = operand1 - operand2;
		break;
	case '*':
		result = operand1 * operand2;
		break;
	case '/':
		result = operand1 / operand2;
		break;
	default:
		return -EINVAL;
	}

	pr_info("Calculating %lld %c %lld\n", operand1, op, operand2);

	result_available = true;

	return sizeof(uint8_t) + (sizeof(int64_t) << 1);
}

MODULE_LICENSE("GPL");
