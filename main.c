#include <linux/init.h>
#include <linux/module.h>

#include "spi.h"

MODULE_AUTHOR("Antoni Przybylik");
MODULE_DESCRIPTION("v1.0.0");
MODULE_LICENSE("GPL");

static int __init hhc_init(void)
{
        int err;

        err = hhc_register();
	if (err) {
        	printk(KERN_ERR "hhc-spi: Error ocurred in init. "
				"Cleaning up.\n");
                return err;
	}

        return 0;
}

static void __exit hhc_exit(void)
{
	hhc_deregister();
}

module_init(hhc_init);
module_exit(hhc_exit);
