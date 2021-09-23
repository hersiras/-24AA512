/*
 * testEeprom.h
 *
 *  Created on: Sep 23, 2021
 *      Author: pills
 */

#ifndef INC_TEST_EEPROM_H_
#define INC_TEST_EEPROM_H_

#include <stdbool.h>
#include <stdint.h>
#include "stm32f1xx_hal.h"

bool test_eeprom_read_write(I2C_HandleTypeDef *i2c, uint8_t i2cChipSelectBits, GPIO_TypeDef *wpPinPort, uint16_t wpPin);


#endif /* INC_TEST_EEPROM_H_ */
