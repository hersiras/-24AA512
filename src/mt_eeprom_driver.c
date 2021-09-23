/*
 * mt_eeprom.c
 *
 *  Created on: Sep 22, 2021
 *      Author: pills
 */
#include "mt_eeprom_driver.h"
#include <stdio.h>

#define I2C_TIMEOUT_MS 100
#define EEPROM_ADDRESS_SIZE_BITS I2C_MEMADD_SIZE_16BIT
#define EEPROM_PAGE_SIZE 128
#define EEPROM_PAGE_COUNT 512
#define EEPROM_DELAY_AFTER_WRITE_MS 5
#define I2C_WRITE 0
#define I2C_READ 1
#define I2C_POLL_DEVICE_READY_TRIALS 5
#define EEPROM_DEFAULT_BYTE_VALUE 0xFF

bool mt_eeprom_read_from_addr(mt_eeprom_handle_t *handle, uint16_t readAddr, uint8_t numBytes, uint8_t *readBuff)
{
	if(handle == NULL || numBytes == 0)
	{
		return false;
	}

	return HAL_I2C_Mem_Read(handle->i2cHandle, handle->i2cAddr, readAddr, EEPROM_ADDRESS_SIZE_BITS, readBuff, numBytes, I2C_TIMEOUT_MS) == HAL_OK;
}

bool mt_eeprom_read_from_page(mt_eeprom_handle_t *handle, uint16_t pageNumber, uint8_t *readBuff, uint8_t numBytes)
{
	if(handle == NULL || numBytes == 0)
	{
		return false;
	}

	uint16_t readAddr = (EEPROM_PAGE_SIZE - 1) * pageNumber;

	return HAL_I2C_Mem_Read(handle->i2cHandle, handle->i2cAddr, readAddr, EEPROM_ADDRESS_SIZE_BITS, readBuff, numBytes, I2C_TIMEOUT_MS) == HAL_OK;
}

bool mt_eeprom_byte_write(mt_eeprom_handle_t *handle, uint16_t writeAddr, uint8_t writeByte)
{
	if(handle == NULL || mt_eeprom_get_write_protect_state(handle) == EEPROM_WP_ON)
	{
		return false;
	}

	//Send data to eeprom
	HAL_StatusTypeDef state = HAL_I2C_Mem_Write(handle->i2cHandle, handle->i2cAddr, writeAddr, EEPROM_ADDRESS_SIZE_BITS, &writeByte, 1, I2C_TIMEOUT_MS) == HAL_OK;
	HAL_Delay(EEPROM_DELAY_AFTER_WRITE_MS);
	return state == HAL_OK;
}

bool mt_eeprom_erase_all(mt_eeprom_handle_t *handle)
{
	if(handle == NULL)
	{
		return false;
	}

	uint8_t defaultValueArray[EEPROM_PAGE_SIZE] = {EEPROM_DEFAULT_BYTE_VALUE};

	for(uint8_t pageIndex = 0; pageIndex < 255; pageIndex++)
	{
		uint16_t eepromAddressToWrite = (EEPROM_PAGE_SIZE - 1) * pageIndex;
		if(HAL_I2C_Mem_Write(handle->i2cHandle, handle->i2cAddr, eepromAddressToWrite, EEPROM_ADDRESS_SIZE_BITS, defaultValueArray, EEPROM_PAGE_SIZE, I2C_TIMEOUT_MS) != HAL_OK)
		{
			//Error writing to page
			return false;
		}
	}

	return true;
}

bool mt_eeprom_page_write(mt_eeprom_handle_t *handle, uint8_t pageNumber, uint8_t *writeBuffer, uint8_t numBytes)
{
	if(handle == NULL || numBytes > EEPROM_PAGE_SIZE || pageNumber > EEPROM_PAGE_COUNT ||
			numBytes == 0 || mt_eeprom_get_write_protect_state(handle) == EEPROM_WP_ON)
	{
		return false;
	}

	uint16_t eepromAddressToWrite = (EEPROM_PAGE_SIZE - 1) * pageNumber;

	//Send data to eeprom
	HAL_StatusTypeDef state = HAL_I2C_Mem_Write(handle->i2cHandle, handle->i2cAddr, eepromAddressToWrite, EEPROM_ADDRESS_SIZE_BITS, writeBuffer, numBytes, I2C_TIMEOUT_MS);
	HAL_Delay(EEPROM_DELAY_AFTER_WRITE_MS);
	return state == HAL_OK;
}

//If pinState is GPIO_PIN_SET, WP is on
void mt_eeprom_set_write_protect_state(mt_eeprom_handle_t *handle, WRITE_PROTECT_STATE state)
{
	HAL_GPIO_WritePin(handle->writeProtectPinPort, handle->writeProtectPin, state);
}

WRITE_PROTECT_STATE mt_eeprom_get_write_protect_state(mt_eeprom_handle_t *handle)
{
	return HAL_GPIO_ReadPin(handle->writeProtectPinPort, handle->writeProtectPin);
}

//If the eeprom is in the middle of a write cycle. Checks for ACK
HAL_StatusTypeDef mt_eeprom_is_ready(mt_eeprom_handle_t *handle)
{
	return HAL_I2C_IsDeviceReady(handle->i2cHandle, handle->i2cAddr, I2C_POLL_DEVICE_READY_TRIALS, I2C_TIMEOUT_MS);
}
