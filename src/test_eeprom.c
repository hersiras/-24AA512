/*
 * testEeprom.c
 *
 *  Created on: Sep 23, 2021
 *      Author: pills
 */

#include "test_eeprom.h"
#include "mt_eeprom_driver.h"
#include <string.h>
#include <stdio.h>

bool test_eeprom_read_write(I2C_HandleTypeDef *i2c, uint8_t i2cChipSelectBits, GPIO_TypeDef *wpPinPort, uint16_t wpPin)
{
	uint8_t testWriteBuf[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
	uint8_t testReadBuf[16] = {0};
	uint16_t pagesToTest[3] = {0, 256, 511};
	uint8_t readWriteByteCount = 16;
	bool errorsOccured = false;

	mt_eeprom_handle_t eepromHandle =
	{
			.i2cHandle = i2c,
			.i2cAddr = ((EEPROM_CONTROL_CODE << 4) | (i2cChipSelectBits << 1)),
			.writeProtectPinPort = wpPinPort,
			.writeProtectPin = wpPin
	};

	//Tests whether we can read and write into eeprom
	mt_eeprom_set_write_protect_state(&eepromHandle, EEPROM_WP_OFF);
	for(uint8_t pagesToTestIndex = 0; pagesToTestIndex < 3; pagesToTestIndex++)
	{
		//Write test buffer
		mt_eeprom_page_write(&eepromHandle, pagesToTest[pagesToTestIndex], testWriteBuf, readWriteByteCount);

		//Then read it
		mt_eeprom_read_from_page(&eepromHandle, pagesToTest[pagesToTestIndex], testReadBuf, readWriteByteCount);

		//If we get different bytes, it's an error
		if(memcmp(testWriteBuf, testReadBuf, sizeof(testWriteBuf)))
		{
			//Can print error msg, assert or whatever on errors
			printf("Error reading, writing and comparing pages");
			errorsOccured = true;
		}

		memset(testReadBuf, 0, sizeof(testReadBuf));
	}
	mt_eeprom_set_write_protect_state(&eepromHandle, EEPROM_WP_ON);

	//Tests whether the write protect works. Write a buffer to eeprom and check whether we read the same values.
	mt_eeprom_set_write_protect_state(&eepromHandle, EEPROM_WP_ON);
	mt_eeprom_page_write(&eepromHandle, pagesToTest[0], testWriteBuf, readWriteByteCount);
	mt_eeprom_read_from_page(&eepromHandle, pagesToTest[0], testReadBuf, readWriteByteCount);
	mt_eeprom_set_write_protect_state(&eepromHandle, EEPROM_WP_OFF);

	if(!memcmp(testWriteBuf, testReadBuf, sizeof(testWriteBuf)))
	{
		printf("Error testing write protect");
		errorsOccured = true;
	}

	//Set all data back to the default byte values
	if(!mt_eeprom_erase_all(&eepromHandle))
	{
		printf("Error erasing all data");
		errorsOccured = true;
	}

	return errorsOccured;
}
