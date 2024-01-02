#ifndef HHC_H_
#define HHC_H_

#include <linux/spi/spi.h>

#define DEVICE_VERSION	"1.0.0"
#define DEVICE_NAME	"Hexapod Hardware Controller v" DEVICE_VERSION

typedef uint16_t angle_t;

#define ANGLE(int_part, float_part) ((angle_t) int_part | \
				     (((angle_t) float_part) << 8))
#define ANGLE_FLOAT_PART(angle)	((uint8_t) (angle >> 8))
#define ANGLE_INT_PART(angle)	((uint8_t) angle)

struct hhc_device {
	struct spi_device *spi;
	struct device	  *dev;
	angle_t servo_angles[6][3];
	bool did_write;

	struct kobject *motion_kobj;
	struct kobject *leg_kobjs[6];
};

#endif /* HHC_H_ */
