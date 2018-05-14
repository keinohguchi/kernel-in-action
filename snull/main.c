/* SPDX-License-Identifier: GPL-2.0 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

/* sn0 and sn1 */
static struct net_device *netdevs[2];

/* snull driver */
struct snull_dev { };

/* net_device_ops */
static int snull_open(struct net_device *dev)
{
	pr_info("%s(%s)\n", __FUNCTION__, netdev_name(dev));
	return 0;
}

static int snull_close(struct net_device *dev)
{
	pr_info("%s(%s)\n", __FUNCTION__, netdev_name(dev));
	return 0;
}

static netdev_tx_t snull_tx(struct sk_buff *skb, struct net_device *dev)
{
	pr_info("%s(%s)\n", __FUNCTION__, netdev_name(dev));
	return NETDEV_TX_OK;
}

const static struct net_device_ops snull_ops = {
	.ndo_open	= snull_open,
	.ndo_stop	= snull_close,
	.ndo_start_xmit	= snull_tx,
};

static void snull_init(struct net_device *dev)
{
	pr_info("%s(%s)\n", __FUNCTION__, netdev_name(dev));
	ether_setup(dev);
	dev->netdev_ops	= &snull_ops;
	dev->flags	|= IFF_NOARP;
}

static int __init snull_init_module(void)
{
	int i;

	pr_info("%s\n", __FUNCTION__);

	for (i = 0; i < ARRAY_SIZE(netdevs); i++) {
		struct net_device *dev;
		int err;

		dev = alloc_netdev(sizeof(struct snull_dev), "sn%d",
				   NET_NAME_UNKNOWN, snull_init);
		if (!dev)
			goto unregister;

		err = register_netdev(dev);
		if (err) {
			free_netdev(dev);
			goto unregister;
		}
		netdevs[i] = dev;
	}
	return 0;
unregister:
	for (i = 0; i < ARRAY_SIZE(netdevs); i++) {
		if (!netdevs[i])
			continue;
		unregister_netdev(netdevs[i]);
		free_netdev(netdevs[i]);
		netdevs[i] = NULL;
	}
	return -ENOMEM;
}
module_init(snull_init_module);

static void __exit snull_cleanup_module(void)
{
	int i;

	pr_info("%s\n", __FUNCTION__);
	for (i = 0; i < ARRAY_SIZE(netdevs); i++) {
		unregister_netdev(netdevs[i]);
		free_netdev(netdevs[i]);
	}
}
module_exit(snull_cleanup_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kei Nohguchi <kei@nohguchi.com>");
MODULE_DESCRIPTION("LDD's Simple Network device");
