#include <linux/module.h>

#include "spi.h"
#include "hhc.h"
#include "fs.h"

static int hhc_probe(struct spi_device *spi);
static void hhc_remove(struct spi_device *spi);

static struct spi_driver hhc_spi_driver = {
	.driver = {
		.name = "hhc-spi",
	},
	.probe	  = hhc_probe,
	.remove	  = hhc_remove,
};

#define MAX_DEVICES 64
struct hhc_device *devices[MAX_DEVICES];
static size_t devices_cnt = 0;

static ssize_t new_hhc_device(struct spi_device *spi)
{
	struct hhc_device *hhc_dev;
	ssize_t device_num = -1;
	size_t i, j;

	/* Too many devices. */
	if (devices_cnt >= MAX_DEVICES) {
		return -1;
	}

	for (i = 0; i < MAX_DEVICES; i++) {
		if (devices[i] == 0) {
			device_num = i;
			break;
		}
	}

	if (device_num < 0) {
		printk(KERN_ERR "hhc-spi: new_hhc_device: Unreachable statement.");
		return -1;
	}

	hhc_dev = kzalloc(sizeof(struct hhc_device), GFP_KERNEL);
	if (!hhc_dev) {
		printk(KERN_ERR "hhc-spi: Out of memory.");
		return -1;
	}
	hhc_dev->spi = spi;
	for (i = 0; i < 6; i++) {
		for (j = 0; j < 3; j++)
			hhc_dev->servo_angles[i][j] = ANGLE(90, 0);
	}

	devices[device_num] = hhc_dev;
	devices_cnt++;

	hhc_dev->dev = fs_create_device_and_config(device_num, hhc_dev);
	if (!hhc_dev->dev) {
		kfree(hhc_dev);
		return -1;
	}

	return device_num;
}

static void destroy_hhc_device(size_t device_num)
{	
	size_t i;

	if (!devices[device_num]) {
		printk(KERN_ERR "hhc-spi: destroy_hhc_device: Attempt to destroy nonexistent device.");
		return;
	}

	for (i = 0; i < 6; i++) {
		if (devices[device_num]->leg_kobjs[i])
			kobject_put(devices[device_num]->leg_kobjs[i]);
	}
	if (devices[device_num]->motion_kobj)
		kobject_put(devices[device_num]->motion_kobj);
	device_unregister(devices[device_num]->dev);
	kfree(devices[device_num]);
	devices[device_num] = 0;
	devices_cnt--;
}

static int hhc_probe(struct spi_device *spi)
{
	ssize_t dev_num;

	dev_num = new_hhc_device(spi);
	if (dev_num < 0) {
		printk(KERN_ERR "hhc-spi: Could not setup sysfs config for device.");
		return -1;
	}

	dev_set_drvdata(&spi->dev, (void*) (dev_num+1));
	return 0;
}

static void hhc_remove(struct spi_device *spi)
{
	ssize_t dev_num;

	dev_num = (size_t) dev_get_drvdata(&spi->dev);
	if (dev_num == 0) {
		printk(KERN_WARNING "hhc-spi: Warning: Bad device.");
		return;
	}

	dev_num -= 1;
	destroy_hhc_device(dev_num);
}

int hhc_register(void)
{
	int err;

	err = fs_class_setup();
	if (err < 0) {
		return err;
	}

	err = spi_register_driver(&hhc_spi_driver);
	if (err < 0) {
		fs_class_destroy();
		return err;
	}

	return 0;
}

void hhc_deregister(void)
{
	spi_unregister_driver(&hhc_spi_driver);
	fs_class_destroy();
}

int send_one_servo_frame(struct spi_device *spi, uint8_t servo_id,
			 uint8_t servo_op_type, angle_t angle)
{
	uint8_t tx[6];
	uint8_t rx[6] = {0};
	struct spi_transfer tr = {
		.tx_buf = &tx,
		.rx_buf = &rx,
		.len = 6,
	};

	tx[0] = 6;  /* FRAME LEN */
	tx[1] = 2;  /* FRAME TYPE */
	tx[2] = servo_id;
	tx[3] = servo_op_type;
	tx[4] = ANGLE_INT_PART(angle);
	tx[5] = ANGLE_FLOAT_PART(angle);

	return spi_sync_transfer(spi, &tr, 1);
}
