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

#ifndef _INPUT_H
#define _INPUT_H

extern "C" {


#include <stdio.h>
#include <gtk/gtk.h>

#include "dt_eeparse.h"

#define MAX_INPUTS (4)

#define DEV_FNAME "/dev/DiamondTouch"

/* default thresholds */
#define COL_THRESHOLD (60)
#define ROW_THRESHOLD (60)

extern unsigned char eebuff[0x40];
extern struct dt_eeparse dt;
extern char *dt_device_name;
extern unsigned *seq;
extern int seq_misses;
extern unsigned last_seq;
extern int largest_seq_diff;
extern int shield_drv_on;

extern int row_threshold;
extern int col_threshold;

/* struct for each user */

struct input_set {
  /* reader stuff */
  int number;
  int iterations;

  /* calc stuff */
  unsigned char *rows, *cols, *sdrv;
  int maxrow, maxcol;
  int bbrowmin, bbrowmax, bbcolmin, bbcolmax;

  /* stat stuff */
  int row_sig_min, row_sig_mean, row_sig_max;
  int col_sig_min, col_sig_mean, col_sig_max;
};

extern struct input_set is[MAX_INPUTS];

int setup_input(char *devname);
void reader_func(gpointer data, gint source, GdkInputCondition condition);
int max_col(unsigned char *rc);
int max_row(unsigned char *rc);
void row_bounding_box(unsigned char *rc, int *mn, int *mx);
void col_bounding_box(unsigned char *rc, int *mn, int *mx);
void process_data(struct input_set *is);
void send_data(struct input_set *is, int num);
void do_display(struct input_set *is);
void calc_stats(struct input_set *is);

};

#endif
