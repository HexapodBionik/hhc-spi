#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/module.h>

#include "hhc.h"
#include "spi.h"

/*
 * Name Attribute
 */
static ssize_t name_show(struct device *dev,
		         struct device_attribute *attr, char *buf)
{
	return sprintf(buf, DEVICE_NAME "\n");
}
static DEVICE_ATTR_RO(name);

/*
 * Hexapod Hardware Controller Device Attributes
 */
static struct attribute *hhc_device_attrs[] = {
	&dev_attr_name.attr,
	NULL,
};
ATTRIBUTE_GROUPS(hhc_device);

/*
 * Hexapod Hardware Controller Device Class
 */
static struct class hhc_class = {
	.name = "hhc",
	.dev_groups = hhc_device_groups,
};

/*
 * Show and Store servo attribute methods
 */
static ssize_t servo_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	struct device *device;
	struct hhc_device *hhc_dev;
	size_t legno, servono;
	angle_t angle;

	legno = attr->attr.name[5] - '1';
	servono = attr->attr.name[6] - '1';

	device = container_of(kobj->parent->parent, struct device, kobj);
	hhc_dev = dev_get_drvdata(device);
	angle = hhc_dev->servo_angles[legno][servono];

	if (!hhc_dev->did_write) {
		if (send_one_servo_frame(hhc_dev->spi, (legno+1)*10 + servono+1,
				         1 /* START & SET */, angle))
			return -1;
		hhc_dev->did_write = true;
	}

	return sprintf(buf, "%hhu.%02hhu\n",\
		       ANGLE_INT_PART(angle),
		       ANGLE_FLOAT_PART(angle));
}

static ssize_t servo_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	size_t i;
	size_t dot_pos = count + 1;
	ssize_t part2len = -1;
	uint8_t angle_int, angle_float;
	angle_t angle;

	struct device *device;
	struct hhc_device *hhc_dev;
	size_t legno, servono;

	for (i = 0; i < count; i++) {
		if (buf[i] == '.') {
			dot_pos = i;
			break;
		}

		if (buf[i] < '0' ||
		    buf[i] > '9')
			return -1;
	}

	if (dot_pos >= count)
		return -1;

	/* Additional validation */
	for (i = dot_pos + 1; i < count; i++) {
		if (buf[i] == '\n') {
			part2len = i - dot_pos - 1;
			break;
		}

		if (buf[i] < '0' ||
		    buf[i] > '9')
			return -1;
	}

	if (part2len < 0)
		part2len = count - dot_pos - 1;

	if (part2len > 2)
		return -1;

	sscanf(buf, "%hhu", (uint8_t*) &angle_int);
	sscanf(buf + dot_pos + 1, "%hhu", (uint8_t*) &angle_float);

	if (part2len == 1)
		angle_float *= 10;

	legno = attr->attr.name[5] - '1';
	servono = attr->attr.name[6] - '1';

	device = container_of(kobj->parent->parent, struct device, kobj);
	hhc_dev = dev_get_drvdata(device);

	angle = ANGLE(angle_int, angle_float);
	hhc_dev->servo_angles[legno][servono] = angle;

	if (!hhc_dev->did_write) {
		if (send_one_servo_frame(hhc_dev->spi, (legno+1)*10 + servono+1,
				         1 /* START & SET */, angle))
			return -1;
		hhc_dev->did_write = true;
	} else {
		if (send_one_servo_frame(hhc_dev->spi, (legno+1)*10 + servono+1,
				         3 /* SET */, angle))
			return -1;
	}

	return count;
}

/*
 * Attributes for servo control
 */
static struct kobj_attribute servo11_attr = __ATTR(servo11, 0664, servo_show, servo_store);
static struct kobj_attribute servo12_attr = __ATTR(servo12, 0664, servo_show, servo_store);
static struct kobj_attribute servo13_attr = __ATTR(servo13, 0664, servo_show, servo_store);
static struct kobj_attribute servo21_attr = __ATTR(servo21, 0664, servo_show, servo_store);
static struct kobj_attribute servo22_attr = __ATTR(servo22, 0664, servo_show, servo_store);
static struct kobj_attribute servo23_attr = __ATTR(servo23, 0664, servo_show, servo_store);
static struct kobj_attribute servo31_attr = __ATTR(servo31, 0664, servo_show, servo_store);
static struct kobj_attribute servo32_attr = __ATTR(servo32, 0664, servo_show, servo_store);
static struct kobj_attribute servo33_attr = __ATTR(servo33, 0664, servo_show, servo_store);
static struct kobj_attribute servo41_attr = __ATTR(servo41, 0664, servo_show, servo_store);
static struct kobj_attribute servo42_attr = __ATTR(servo42, 0664, servo_show, servo_store);
static struct kobj_attribute servo43_attr = __ATTR(servo43, 0664, servo_show, servo_store);
static struct kobj_attribute servo51_attr = __ATTR(servo51, 0664, servo_show, servo_store);
static struct kobj_attribute servo52_attr = __ATTR(servo52, 0664, servo_show, servo_store);
static struct kobj_attribute servo53_attr = __ATTR(servo53, 0664, servo_show, servo_store);
static struct kobj_attribute servo61_attr = __ATTR(servo61, 0664, servo_show, servo_store);
static struct kobj_attribute servo62_attr = __ATTR(servo62, 0664, servo_show, servo_store);
static struct kobj_attribute servo63_attr = __ATTR(servo63, 0664, servo_show, servo_store);

static struct attribute *leg1_attrs[] = {
	&servo11_attr.attr,
	&servo12_attr.attr,
	&servo13_attr.attr,
	NULL,
};

static struct attribute *leg2_attrs[] = {
	&servo21_attr.attr,
	&servo22_attr.attr,
	&servo23_attr.attr,
	NULL,
};

static struct attribute *leg3_attrs[] = {
	&servo31_attr.attr,
	&servo32_attr.attr,
	&servo33_attr.attr,
	NULL,
};

static struct attribute *leg4_attrs[] = {
	&servo41_attr.attr,
	&servo42_attr.attr,
	&servo43_attr.attr,
	NULL,
};

static struct attribute *leg5_attrs[] = {
	&servo51_attr.attr,
	&servo52_attr.attr,
	&servo53_attr.attr,
	NULL,
};

static struct attribute *leg6_attrs[] = {
	&servo61_attr.attr,
	&servo62_attr.attr,
	&servo63_attr.attr,
	NULL,
};

static struct attribute_group leg1_attr_group = {
	.attrs = leg1_attrs,
};

static struct attribute_group leg2_attr_group = {
	.attrs = leg2_attrs,
};

static struct attribute_group leg3_attr_group = {
	.attrs = leg3_attrs,
};

static struct attribute_group leg4_attr_group = {
	.attrs = leg4_attrs,
};

static struct attribute_group leg5_attr_group = {
	.attrs = leg5_attrs,
};

static struct attribute_group leg6_attr_group = {
	.attrs = leg6_attrs,
};

/*
 * Sets up /sys/class/hhc/
 */
int fs_class_setup(void)
{
	int err;

	err = class_register(&hhc_class);
	if (err) {
		printk(KERN_ERR "hhc-spi: Failed to register device class.");
		return -1;
	}

	return 0;
}

/*
 * Destroys hhc device class
 */
void fs_class_destroy(void)
{
	class_unregister(&hhc_class);
}

static void empty_release(struct device *device)
{
}

/*
 * Creates new device.
 * (and sets up /sys/class/hhc/hhc<hhc_num>/)
 */
struct device *fs_create_device_and_config(int8_t hhc_num, struct hhc_device *hhc_dev)
{
	char name[20];
	struct device *device;

	device = kzalloc(sizeof(struct device), GFP_KERNEL);
	if (!device) {
		printk(KERN_ERR "hhc-spi: Out of memory.");
		return 0;
	}
	device->class = &hhc_class;

	snprintf(name, 20, "hhc%d", (int32_t) hhc_num);
	device->kobj.name = kzalloc(strlen(name)+1, GFP_KERNEL);
	strcpy((char*) device->kobj.name, name);
	device->release = empty_release;
	dev_set_drvdata(device, hhc_dev);
	if (device_register(device) < 0) {
		printk(KERN_ERR "hhc-spi: Could not register device.");
		return 0;
	}

	hhc_dev->motion_kobj = kobject_create_and_add("motion", &device->kobj);

	/* Leg 1 */
	hhc_dev->leg_kobjs[0] =
		kobject_create_and_add("leg1", hhc_dev->motion_kobj);
	if (sysfs_create_group(hhc_dev->leg_kobjs[0], &leg1_attr_group)) {
		kobject_put(hhc_dev->leg_kobjs[0]);
		return 0;
	}

	/* Leg 2 */
	hhc_dev->leg_kobjs[1] =
		kobject_create_and_add("leg2", hhc_dev->motion_kobj);
	if (sysfs_create_group(hhc_dev->leg_kobjs[1], &leg2_attr_group)) {
		kobject_put(hhc_dev->leg_kobjs[1]);
		return 0;
	}

	/* Leg 3 */
	hhc_dev->leg_kobjs[2] =
		kobject_create_and_add("leg3", hhc_dev->motion_kobj);
	if (sysfs_create_group(hhc_dev->leg_kobjs[2], &leg3_attr_group)) {
		kobject_put(hhc_dev->leg_kobjs[2]);
		return 0;
	}

	/* Leg 4 */
	hhc_dev->leg_kobjs[3] =
		kobject_create_and_add("leg4", hhc_dev->motion_kobj);
	if (sysfs_create_group(hhc_dev->leg_kobjs[3], &leg4_attr_group)) {
		kobject_put(hhc_dev->leg_kobjs[3]);
		return 0;
	}

	/* Leg 5 */
	hhc_dev->leg_kobjs[4] =
		kobject_create_and_add("leg5", hhc_dev->motion_kobj);
	if (sysfs_create_group(hhc_dev->leg_kobjs[4], &leg5_attr_group)) {
		kobject_put(hhc_dev->leg_kobjs[4]);
		return 0;
	}

	/* Leg 6 */
	hhc_dev->leg_kobjs[5] =
		kobject_create_and_add("leg6", hhc_dev->motion_kobj);
	if (sysfs_create_group(hhc_dev->leg_kobjs[5], &leg6_attr_group)) {
		kobject_put(hhc_dev->leg_kobjs[5]);
		return 0;
	}
	
	return device;
}
