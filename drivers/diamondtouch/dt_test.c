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


#include <gtk/gtk.h>
#include <glade/glade.h>

#include "callbacks.h"
#include "input.h"

void usage() { fprintf(stderr, "Usage: dt_test <device>\n"); }

char *devname = NULL;

int
main(int argc, char *argv[]) {
  int ret;
  GladeXML *xml;

  gtk_init(&argc, &argv);

  switch (argc) {
  case 1: devname = DEV_FNAME; break;
  case 2: devname = argv[1]; break;
  default: usage(); gtk_exit(-1); break;
  }

  xml = glade_xml_new("dt_test.glade", NULL, NULL);

  /* connect signal handlers */
  glade_xml_signal_autoconnect(xml);

  ret = setup_input(devname);
  if (ret < 0) {
    fprintf(stderr, "Device %s could not be used.\n", devname);
    gtk_exit(-2);
  }

  setup_widgets(xml);

  gtk_main();

  return 0;
}
