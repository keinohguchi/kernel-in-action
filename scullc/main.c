/* SPDX-License-Identifier: GPL-2.0 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>

#include "../ldd/ldd.h"

#define SCULLC_DRIVER_VERSION		"1.0"
#define SCULLC_DRIVER_NAME		"scullc"
#define SCULLC_DEFAULT_QUANTUM_SIZE	PAGE_SIZE

static const char *driver_version = SCULLC_DRIVER_VERSION;
static const char *driver_name = SCULLC_DRIVER_NAME;
static int quantum_size = SCULLC_DEFAULT_QUANTUM_SIZE;
module_param(quantum_size, int, S_IRUGO);

static struct ldd_driver scullc_driver = {
	.module = THIS_MODULE,
};

static struct scullc_device {
	struct ldd_device	dev;
	struct cdev		cdev;
} scullc_devices[] = {
	{ .dev.name = SCULLC_DRIVER_NAME "0" },
	{ .dev.name = SCULLC_DRIVER_NAME "1" },
	{ .dev.name = SCULLC_DRIVER_NAME "2" },
	{ .dev.name = SCULLC_DRIVER_NAME "3" },
	{ /* sentry */ },
};

/* kmem cache */
static struct kmem_cache *quantum_cache;

static ssize_t read(struct file *f, char __user *buf, size_t n, loff_t *pos)
{
	return 0;
}

static ssize_t write(struct file *f, const char __user *buf, size_t n, loff_t *pos)
{
	return 0;
}

static int open(struct inode *i, struct file *f)
{
	return 0;
}

static int release(struct inode *i, struct file *f)
{
	return 0;
}

static const struct file_operations fops = {
	.owner		= THIS_MODULE,
	.read		= read,
	.write		= write,
	.open		= open,
	.release	= release,
};

static int __init init(void)
{
	struct scullc_device *d, *del;
	int err;

	pr_info("%s\n", __FUNCTION__);

	quantum_cache = kmem_cache_create("scullc_quantum", quantum_size,
					  0, SLAB_HWCACHE_ALIGN, NULL);
	if (!quantum_cache)
		return -ENOMEM;

	scullc_driver.version = driver_version;
	scullc_driver.driver.name = driver_name;
	err = register_ldd_driver(&scullc_driver);
	if (err)
		goto destroy_cache;

	for (d = scullc_devices; d->dev.name; d++) {
		cdev_init(&d->cdev, &fops);
		err = register_ldd_device(&d->dev);
		if (err)
			goto unregister;
	}
	return 0;
unregister:
	for (del = scullc_devices; del != d; del++)
		unregister_ldd_device(&del->dev);
	unregister_ldd_driver(&scullc_driver);
destroy_cache:
	if (quantum_cache)
		kmem_cache_destroy(quantum_cache);
	return err;
}
module_init(init);

static void __exit cleanup(void)
{
	struct scullc_device *d;

	pr_info("%s\n", __FUNCTION__);

	for (d = scullc_devices; d->dev.name; d++)
		unregister_ldd_device(&d->dev);
	unregister_ldd_driver(&scullc_driver);
	if (quantum_cache)
		kmem_cache_destroy(quantum_cache);
}
module_exit(cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kei Nohguchi <kei@nohguchi.com>");
MODULE_DESCRIPTION("kmem_cache_alloc() example");
