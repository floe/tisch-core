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


#include "callbacks.h"
#include "input.h"
#include "diamondtouch.h"

#include "BasicBlob.h"
#include "UDPSender.h"

#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int col_threshold = COL_THRESHOLD;
int row_threshold = ROW_THRESHOLD;

unsigned char eebuff[0x40];
struct dt_eeparse dt;
char *dt_device_name;

unsigned char *buff;
unsigned *seq;
int seq_misses;
unsigned last_seq;
int largest_seq_diff;
int shield_drv_on;

struct input_set is[MAX_INPUTS];

UDPSender udp( INADDR_LOOPBACK, 0x7AB1 - 1 );
int* ids;
int currid = 0;


int setup_input(char *devname) {
  int i, fd, ret;
  int rowscols;

  dt_device_name = devname;

  fd = open(devname, O_RDONLY | O_NONBLOCK);
  if (fd < 0) {
    fprintf(stderr, "Could not open device %s\n", devname);
    return fd;
  }

  ret = ioctl(fd, DT_IOCGPARAMS, eebuff);
  if (ret < 0) {
    perror("ioctl");
    fprintf(stderr, "ioctl failed with ret = %d\n", ret);
    return -2;
  }

  dt_parse_params(eebuff, &dt);

  rowscols = dt.num_rows + dt.num_columns;
  if (dt.shield_drv_on) rowscols++;

  buff = (unsigned char *) malloc(dt.read_size);
  if (buff == NULL) {
    fprintf(stderr, "Could not allocate read buffer.\n");
    return -3;
  }

  largest_seq_diff = -1;
  seq_misses = -1;
  last_seq = 0;
  seq = (unsigned *) buff; /* point to the sequence number at the beginning */

  gdk_input_add (fd, GDK_INPUT_READ, reader_func, (gpointer) (is));

	ids = (int*)malloc(sizeof(int)*dt.num_users);
  for (i = 0; i < dt.num_users; i++) {
		ids[i] = 0;
    is[i].iterations = 0;
    is[i].number = i;
    if (dt.rows_first) {
      is[i].rows = buff + 4 + i * rowscols;
      is[i].cols = is[i].rows + dt.num_rows;
      is[i].sdrv = is[i].cols + dt.num_columns;
    }
    else {
      is[i].cols = buff + 4 + i * rowscols;
      is[i].rows = is[i].cols + dt.num_columns;
      is[i].sdrv = is[i].rows + dt.num_rows;
    }
  }
  return 0;
}

void reader_func(gpointer data, gint source, GdkInputCondition condition) {
  int ret, i;
  int seq_diff;
  struct input_set *is = (struct input_set *) data;

  ret = read(source, buff, dt.read_size);
  if (ret < 0) {
    if (errno == EAGAIN) {
      fprintf(stderr, "reader func would block -- ??\n");
      return;
    }
    else {
      gtk_main_quit();
      return;
    }
  }
  if (ret != dt.read_size) {
    fprintf(stderr, "read returned %d instead of %d\n", ret, dt.read_size);
  }

  if (seq_misses == -1) {
    seq_misses = 0;
    largest_seq_diff = -1;
  }
  else {
    seq_diff = *seq - last_seq - 1;
    if (seq_diff > largest_seq_diff) largest_seq_diff = seq_diff;
    seq_misses += seq_diff;    
  }
  last_seq = *seq;

  for (i = 0; i < dt.num_users; i++) {
    process_data(is + i);
    display_data(is + i);
		send_data(is + i,i);
  }
  recycle_display();
}

/* calculations to perform on the input data */

void send_data(struct input_set *is, int num) {

	BasicBlob tmp;

	if ((is->bbrowmin == -1) || (is->bbrowmax == -1)) { ids[num] = 0; currid++; return; }
	if ((is->bbcolmin == -1) || (is->bbcolmax == -1)) { ids[num] = 0; currid++; return; }

	if (ids[num] == 0) ids[num] = currid;

	int size = (is->bbrowmax - is->bbrowmin)*(is->bbcolmax - is->bbcolmin);

	double x = (is->bbcolmax + is->bbcolmin)/2.0;
	double y = (is->bbrowmax + is->bbrowmin)/2.0;

	tmp.size = size;
	tmp.pid = 0;
	tmp.id = ids[num];

	tmp.pos   = Vector(x,y);
	tmp.peak  = Vector(x,y);
	tmp.axis1 = Vector(0,0);

	udp << tmp;
}


void process_data(struct input_set *is) {
  is->maxrow = max_row(is->rows);
  is->maxcol = max_col(is->cols);
  row_bounding_box(is->rows, &is->bbrowmin, &is->bbrowmax);
  col_bounding_box(is->cols, &is->bbcolmin, &is->bbcolmax);
  calc_stats(is);
  is->iterations++;
}


int max_row(unsigned char *rc) {
  int i, mx, ret;

  ret = mx = -1;
  for (i = 0; i < dt.num_rows; i++) {
    if (rc[i] > mx) {
      mx = rc[i];
      ret = i;
    }
  }
  if (mx < row_threshold) ret = -1;
  return ret;
}

int max_col(unsigned char *rc) {
  int i, mx, ret;

  ret = mx = -1;
  for (i = 0; i < dt.num_columns; i++) {
    if (rc[i] > mx) {
      mx = rc[i];
      ret = i;
    }
  }
  if (mx < col_threshold) ret = -1;
  return ret;
}

void row_bounding_box(unsigned char *rc, int *mn, int *mx) {
  int i;

  *mn = *mx = -1;

  for (i = 0; i < dt.num_rows; i++) {
    if (rc[i] >= row_threshold) {
      if (*mn == -1) *mn = i;
      *mx = i;
    }
  }
}

void col_bounding_box(unsigned char *rc, int *mn, int *mx) {
  int i;

  *mn = *mx = -1;

  for (i = 0; i < dt.num_columns; i++) {
    if (rc[i] >= col_threshold) {
      if (*mn == -1) *mn = i;
      *mx = i;
    }
  }
}

void calc_stats(struct input_set *is) {
  int i;
  int min, max, mean;
  int sample;

  min = 9999; max = -1; mean = 0;
  for (i = 0; i < dt.num_columns; i++) {
    sample = is->cols[i];
    mean += sample;
    if (sample > max) max = sample;
    if (sample < min) min = sample;
  }
  is->col_sig_min = min;
  is->col_sig_max = max;
  is->col_sig_mean = mean / dt.num_columns;
  
  
  min = 9999; max = -1; mean = 0;
  for (i = 0; i < dt.num_rows; i++) {
    sample = is->rows[i];
    mean += sample;
    if (sample > max) max = sample;
    if (sample < min) min = sample;
  }
  is->row_sig_min = min;
  is->row_sig_max = max;
  is->row_sig_mean = mean / dt.num_rows;
}
