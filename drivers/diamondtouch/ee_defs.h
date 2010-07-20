/*************************************************************************\

  Copyright 2001, 2002, 2003, 2004, 2005, 2006
  Mitsubishi Electric Research Laboratories.
  All rights reserved.

     Permission to use, copy and modify this software and its
     documentation without fee for educational, research and non-profit
     purposes, is hereby granted, provided that the above copyright
     notice and the following three paragraphs appear in all copies.

     To request permission to incorporate this software into commercial
     products contact:  Vice President of Marketing and Business Development;
     Mitsubishi Electric Research Laboratories (MERL), 201 Broadway,
     Cambridge, MA   02139 or <license@merl.com>.

     IN NO EVENT SHALL MERL BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
     SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS,
     ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
     MERL HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

     MERL SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED
     TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
     PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
     BASIS, AND MERL HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT,
     UPDATES, ENHANCEMENTS OR MODIFICATIONS.

  Darren Leigh (leigh@merl.com) wrote and is responsible for this program.

\*************************************************************************/

#ifndef __EE_DEFS_H
#define __EE_DEFS_H



/* DT eeprom location addresses */
#define DT_EE_BOOT_BYTE			0x00
#define DT_EE_VENDOR_ID_LSB		0x01
#define DT_EE_VENDOR_ID_MSB		0x02
#define DT_EE_PRODUCT_ID_LSB		0x03
#define DT_EE_PRODUCT_ID_MSB		0x04
#define DT_EE_DEVICE_ID_LSB		0x05
#define DT_EE_DEVICE_ID_MSB		0x06
#define DT_EE_CONFIG_BYTE		0x07
#define DT_EE_RESERVED_0		0x08
#define DT_EE_EXTENSION_VERSION		0x09
#define DT_EE_RENUM_VENDOR_ID_LSB	0x0a
#define DT_EE_RENUM_VENDOR_ID_MSB	0x0b
#define DT_EE_RENUM_PRODUCT_ID_LSB	0x0c
#define DT_EE_RENUM_PRODUCT_ID_MSB	0x0d
#define DT_EE_RENUM_DEVICE_ID_LSB	0x0e
#define DT_EE_RENUM_DEVICE_ID_MSB	0x0f

/* version 0 and 1 definitions */
#define DT_EE_CALIBRATION_CONST_0	0x10
#define DT_EE_CALIBRATION_CONST_1	0x11
#define DT_EE_CALIBRATION_CONST_2	0x12
#define DT_EE_CALIBRATION_CONST_3	0x13
#define DT_EE_CALIBRATION_CONST_4	0x14
#define DT_EE_CALIBRATION_CONST_5	0x15
#define DT_EE_CALIBRATION_CONST_6	0x16
#define DT_EE_CALIBRATION_CONST_7	0x17

/* version 2 definitions */
#define DT_EE_MDAC_LOW_COLUMNS		0x10
#define DT_EE_MDAC_HIGH_COLUMNS		0x11
#define DT_EE_MDAC_LOW_ROWS		0x12
#define DT_EE_MDAC_HIGH_ROWS		0x13
#define DT_EE_MDAC_LOW_SHIELD_DRV	0x14
#define DT_EE_MDAC_HIGH_SHIELD_DRV	0x15
#define DT_EE_SHIELD_DRV_FLAGS		0x16

#define DT_EE_PIC_FIRMWARE_VERSION	0x18
#define DT_EE_NUM_SUPPORTED_USERS	0x19
#define DT_EE_NUM_COLUMNS_LSB		0x1a
#define DT_EE_NUM_COLUMNS_MSB		0x1b
#define DT_EE_NUM_ROWS_LSB		0x1c
#define DT_EE_NUM_ROWS_MSB		0x1d
#define DT_EE_UPDATE_PERIOD_LSB		0x1e
#define DT_EE_UPDATE_PERIOD_MSB		0x1f

#define DT_EE_COLUMN_WIDTH_LSB		0x20
#define DT_EE_COLUMN_WIDTH_MSB		0x21
#define DT_EE_ROW_WIDTH_LSB		0x22
#define DT_EE_ROW_WIDTH_MSB		0x23
#define DT_EE_ROW_COLUMN_ORIENTATION	0x24
#define DT_EE_PIPE_INTERVAL		0x25
#define DT_EE_SPARE_1			0x26
#define DT_EE_SPARE_2			0x27
#define DT_EE_MANUFACTURE_YEAR_1	0x28
#define DT_EE_MANUFACTURE_YEAR_2	0x29
#define DT_EE_MANUFACTURE_YEAR_3	0x2a
#define DT_EE_MANUFACTURE_YEAR_4	0x2b
#define DT_EE_MANUFACTURE_MONTH_1	0x2c
#define DT_EE_MANUFACTURE_MONTH_2	0x2d
#define DT_EE_MANUFACTURE_DAY_1		0x2e
#define DT_EE_MANUFACTURE_DAY_2		0x2f

#define DT_EE_UNIQUE_SERIALNUM_0	0x30
#define DT_EE_UNIQUE_SERIALNUM_1	0x31
#define DT_EE_UNIQUE_SERIALNUM_2	0x32
#define DT_EE_UNIQUE_SERIALNUM_3	0x33
#define DT_EE_UNIQUE_SERIALNUM_4	0x34
#define DT_EE_UNIQUE_SERIALNUM_5	0x35
#define DT_EE_UNIQUE_SERIALNUM_6	0x36
#define DT_EE_UNIQUE_SERIALNUM_7	0x37
#define DT_EE_PIC_CONFIG_NUM_ADS	0x38
#define DT_EE_PIC_CONFIG_NUM_CYCLES	0x39
#define DT_EE_PIC_CONFIG_ROWCOLS_LSB	0x3a
#define DT_EE_PIC_CONFIG_ROWCOLS_MSB	0x3b
#define DT_EE_PIC_CONFIG_COLUMNS_LSB	0x3c
#define DT_EE_PIC_CONFIG_COLUMNS_MSB	0x3d

/*** various bit definitions ***/

/* bit definitions for DT_EE_SHIELD_DRV_FLAGS (version 2) */
#define DT_EE_SHIELD_DRV_ON		0x01

/* bit definitions for DT_EE_ROW_COLUMN_ORIENTATION */
#define DT_EE_ROWS_FIRST		0x01
#define DT_EE_COLUMNS_LEFT_TO_RIGHT	0x02
#define DT_EE_ROWS_BOTTOM_TO_TOP	0x04

/* bit definitions for DT_EE_PIC_CONFIG_ROWCOLS_MSB */
#define DT_EE_PIC_ROWCOLS_MSB_ZERO	0x80
#define DT_EE_PIC_ROWCOLS_LSB_ZERO	0x40

#define DT_EE_TYPE_MINIMUM		0x00
#define DT_EE_TYPE_DT88			0x00
#define DT_EE_TYPE_DT107		0x01
#define DT_EE_TYPE_DT5X3		0x02
#define DT_EE_TYPE_EXP			0x03
#define DT_EE_TYPE_DT81M4		0x04
#define DT_EE_TYPE_DT107M4		0x05
#define DT_EE_TYPE_UNUSED		0xff
#define DT_EE_TYPE_MAXIMUM		0x05 /* currently */

#endif /* __EE_DEFS_H */
