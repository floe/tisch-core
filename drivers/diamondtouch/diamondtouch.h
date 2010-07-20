/*************************************************************************\

    Copyright 2001, 2002, 2003, 2004, 2005, 2006
    Mitsubishi Electric Research Laboratories.
    All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
    02110-1301  USA

    Darren Leigh (leigh@merl.com) wrote and is responsible for this program.

\*************************************************************************/

#ifndef _DIAMONDTOUCH_H
#define _DIAMONDTOUCH_H



/* do we need these two? */
#include <linux/types.h>
#include <linux/ioctl.h>

#include "ee_defs.h"

#define VENDID	0x06d3
#define PRODID	0x0f81

/* ioctl definitions */

#define DT_IOC_MAGIC  0xdd

#define DT_IOCGPARAMS _IOC(_IOC_READ,  DT_IOC_MAGIC, 1, 0x40)
#define DT_IOCSPARAMS _IOC(_IOC_WRITE, DT_IOC_MAGIC, 2, 0x40)

#ifdef __KERNEL__

#include <linux/device.h>
#include <linux/proc_fs.h>
#include <asm/semaphore.h>

/* these are constants because of current hardware constraints */
#define OUT_BUFFSIZE	0x40
#define PARAM_BUFFSIZE	0x40
#define USER_BUFFSIZE	0x40
#define BUFF_HEADERSIZE	(4)
#define MAX_DEVICES	(4)

/* this is the per-user data for the device */
struct dt_per_user {
  struct urb *irq_urb;
  unsigned dudata_subframe;
  u8 userbuf[USER_BUFFSIZE];
  volatile u8 * volatile dudata;
};

/* this is the data for each instance of diamondtouch device */
struct dt_per_device {
  struct semaphore sem;
  struct usb_device *udev;
  struct class_device *cd;
  unsigned minor;
  unsigned active;
  unsigned moribund;

  /* device parameters */
  u8 param_buff[PARAM_BUFFSIZE];
  unsigned num_users;
  unsigned read_size;
  unsigned num_samples;
  unsigned num_subframes;
  unsigned last_payload;
  unsigned all_subframes_received;
  unsigned frame_incer;
  
  /* Bulk Transfers */
  u8 out_buff[OUT_BUFFSIZE];

  /* Interrupt Transfers */
  wait_queue_head_t irq_q;
  unsigned interval;

  /* device user data */
  struct dt_per_user *user; /* must kmalloc these */
  unsigned dudata_unread;
  unsigned dudata_frame;
  u8 *dbuff; /* the data buffers (ptr for mallocing and freeing) */
  volatile u8 * volatile fbuff; /* buffer that is currently being filled */
  volatile u8 * volatile ubuff; /* ultimate buffer (latest data) */
  volatile u8 * volatile pbuff; /* penultimate buffer (next-to-latest data) */
  volatile long latest_data_available;

  /* list of per-open data for this device */
  struct list_head per_open_list;
  int open_count;
};

/* this is the per-open data */
struct dt_per_open {
  struct list_head list;
  long last_data_sent;
  struct dt_per_device *dtpd;
};

/* forward declarations */
static void dt_irq_start(struct dt_per_device *dt);
static void dt_irq_stop(struct dt_per_device *dt);
static struct file_operations dt_file_operations;
static void dt_irq_irq(struct urb *murb, struct pt_regs *regs);
static int dt_parse_params(struct dt_per_device *dt);
static void init_minors(void);
static void free_minor(struct dt_per_device *dt);
static int map_minor(struct dt_per_device *dt);
static void rotate_buffers(struct dt_per_device *dt);
static int read_proc_dt(char *buf, char **start, off_t offset,
			int len, int *eof, void *data);

#define DT_STANDARD_TIMEOUT	(HZ/2)

/* pipe definitions */
#define DT_USER1_PIPE	(1)
#define DT_USER2_PIPE	(2)
#define DT_USER3_PIPE	(3)
#define DT_USER4_PIPE	(4)
#define DT_OUT_PIPE	(5)
#define DT_PARAM_PIPE	(6)

#define DT_FIRST_PACKET_PAYLOAD		(58)
#define DT_SUBSEQUENT_PACKET_PAYLOAD	(62)

/* custom kernel printing macros */
#ifdef DEBUG
#define dt_dbg(format, arg...) printk(KERN_DEBUG format , ## arg)
#else
#define dt_dbg(format, arg...) do {} while (0)
#endif

#define dt_err(format, arg...) printk(KERN_ERR format , ## arg)
#define dt_info(format, arg...) printk(KERN_INFO format , ## arg)
#define dt_warn(format, arg...) printk(KERN_WARNING format , ## arg)


#endif /* __KERNEL__ */
#endif /* _DIAMONDTOUCH_H */
