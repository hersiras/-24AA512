/*
 * mt_eeprom_driver.h
 *
 *  Created on: Sep 22, 2021
 *      Author: pills
 */

#ifndef INC_MT_EEPROM_DRIVER_H_
#define INC_MT_EEPROM_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f1xx_hal.h"

#define EEPROM_CONTROL_CODE 0b1010

typedef enum WRITE_PROTECT_STATE
{
	EEPROM_WP_ON = GPIO_PIN_SET,
	EEPROM_WP_OFF = GPIO_PIN_RESET
} WRITE_PROTECT_STATE;

typedef struct mt_eeprom_handle_t
{
	I2C_HandleTypeDef *i2cHandle;
	uint8_t i2cAddr; //control code + last 3 bits of "slave address", 0..8
	GPIO_TypeDef *writeProtectPinPort;
	uint16_t writeProtectPin;
} mt_eeprom_handle_t;

bool mt_eeprom_read_from_addr(mt_eeprom_handle_t *handle, uint16_t readAddr, uint8_t numBytes, uint8_t *readBuff);
bool mt_eeprom_read_from_page(mt_eeprom_handle_t *handle, uint16_t pageNumber, uint8_t *readBuff, uint8_t numBytes);
bool mt_eeprom_byte_write(mt_eeprom_handle_t *handle, uint16_t writeAddr, uint8_t writeByte);
bool mt_eeprom_erase_all(mt_eeprom_handle_t *handle);
bool mt_eeprom_page_write(mt_eeprom_handle_t *handle, uint8_t pageNumber, uint8_t *writeBuffer, uint8_t numBytes);
void mt_eeprom_set_write_protect_state(mt_eeprom_handle_t *handle, WRITE_PROTECT_STATE state);
WRITE_PROTECT_STATE mt_eeprom_get_write_protect_state(mt_eeprom_handle_t *handle);
HAL_StatusTypeDef mt_eeprom_is_ready(mt_eeprom_handle_t *handle);

#endif /* INC_MT_EEPROM_DRIVER_H_ */
