/***********************************************************************

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

*************************************************************************/


/* parse DiamondTouch eeprom parameters */

#include "dt_eeparse.h"

char *dt_types[] = {
  "DT88_",
  "DT107_",
  "DT5X3_",
  "exp_",
  "DT81M4_",
  "DT107M4_"
};

/* Parse data from the DiamondTouch eeprom buffer. */
void
dt_parse_params(unsigned char *ee, struct dt_eeparse *dteep) {
  dteep->num_users = ee[DT_EE_NUM_SUPPORTED_USERS];
  dteep->num_columns = ee[DT_EE_NUM_COLUMNS_MSB] << 8 |
    ee[DT_EE_NUM_COLUMNS_LSB];
  dteep->num_rows = ee[DT_EE_NUM_ROWS_MSB] << 8 | ee[DT_EE_NUM_ROWS_LSB];
  dteep->update_period = (ee[DT_EE_UPDATE_PERIOD_MSB] << 8 |
			  ee[DT_EE_UPDATE_PERIOD_LSB]) / 1.0e5;
  dteep->column_width = (ee[DT_EE_COLUMN_WIDTH_MSB] << 8 |
			 ee[DT_EE_COLUMN_WIDTH_LSB]) / 1.0e5;
  dteep->row_width = (ee[DT_EE_ROW_WIDTH_MSB] << 8 |
		      ee[DT_EE_ROW_WIDTH_LSB]) / 1.0e5;
  dteep->rows_first = ((ee[DT_EE_ROW_COLUMN_ORIENTATION] & DT_EE_ROWS_FIRST)
		       == DT_EE_ROWS_FIRST);
  dteep->columns_left_to_right = ((ee[DT_EE_ROW_COLUMN_ORIENTATION] &
				   DT_EE_COLUMNS_LEFT_TO_RIGHT)
				  == DT_EE_COLUMNS_LEFT_TO_RIGHT);
  dteep->rows_top_to_bottom = ((ee[DT_EE_ROW_COLUMN_ORIENTATION] &
				   DT_EE_ROWS_BOTTOM_TO_TOP)
				  != DT_EE_ROWS_BOTTOM_TO_TOP);

  dteep->read_size = 4 + dteep->num_users *
    (dteep->num_columns + dteep->num_rows);

  dteep->pic_version = ee[DT_EE_PIC_FIRMWARE_VERSION];

  dteep->shield_drv_on = 0;
  if (dteep->pic_version >= 2) {
    /* add extra row/column for shield drive value if needed */
    if ((ee[DT_EE_SHIELD_DRV_FLAGS] & DT_EE_SHIELD_DRV_ON) != 0) {
      dteep->shield_drv_on = 1;
      dteep->read_size += dteep->num_users;
    }
  }
}
