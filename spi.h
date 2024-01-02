#ifndef SPI_H_
#define SPI_H_

#include "hhc.h"

int hhc_register(void);
void hhc_deregister(void);

int send_one_servo_frame(struct spi_device *spi, uint8_t servo_id,
			 uint8_t servo_op_type, angle_t angle);

#endif /* SPI_H_ */
