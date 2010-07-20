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

#ifndef __DT_EEPARSE_H
#define __DT_EEPARSE_H

extern "C" {


#include "ee_defs.h"

extern char *dt_types[];

struct dt_eeparse {
  int num_users;
  int num_columns;
  int num_rows;
  float update_period; /* in seconds */
  float column_width;  /* in meters */
  float row_width;     /* in meters */
  int rows_first;		/* boolean */
  int columns_left_to_right;	/* boolean */
  int rows_top_to_bottom;	/* boolean */
  int read_size;
  int pic_version;
  int shield_drv_on;
};

void dt_parse_params(unsigned char *ee, struct dt_eeparse *dteep);

};

#endif /* __DT_EEPARSE_H */
