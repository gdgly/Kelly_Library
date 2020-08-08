/*
 * Bootloader.h
 *
 *  Created on: Jul 31, 2014
 *      Author: Admin
 */

#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_

byte Boot_SCI1_RecvChar(byte *Chr);
byte Boot_SCI1_SendChar(byte Chr);
void boot_init(void);
void boot(void);

void boot_Init_WDOG(void);

void boot_WDOG_Feed(void);

#endif /* BOOTLOADER_H_ */
