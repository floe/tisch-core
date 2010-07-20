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

#ifndef _CALLBACKS_H
#define _CALLBACKS_H

extern "C" {



#include <gtk/gtk.h>
#include <glade/glade.h>

#include "input.h"

G_MODULE_EXPORT gboolean
on_columns_drawingarea_expose_event(GtkWidget *widget,
				    GdkEventExpose *event, gpointer user_data);
G_MODULE_EXPORT gboolean
on_middle_drawingarea_expose_event(GtkWidget *widget,
				   GdkEventExpose *event, gpointer user_data);
G_MODULE_EXPORT gboolean
on_rows_drawingarea_expose_event(GtkWidget *widget,
				 GdkEventExpose *event, gpointer user_data);

G_MODULE_EXPORT void
on_row_threshold_scale_value_changed(GtkRange *range, gpointer user_data);
G_MODULE_EXPORT void
on_column_threshold_scale_value_changed(GtkRange *range, gpointer user_data);
G_MODULE_EXPORT void
on_threshold_lock_checkbutton_toggled(GtkToggleButton *togglebutton,
				      gpointer user_data);

G_MODULE_EXPORT void
on_info_button_clicked(GtkButton* button, gpointer user_data);
G_MODULE_EXPORT void
on_debug_button_clicked(GtkButton* button, gpointer user_data);
G_MODULE_EXPORT void
on_stats_button_clicked(GtkButton* button, gpointer user_data);
G_MODULE_EXPORT void
on_about_button_clicked(GtkButton* button, gpointer user_data);

void setup_widgets(GladeXML *xml);
void set_description_label();
void display_data(struct input_set *is);
void recycle_display(void);

};

#endif /* _CALLBACKS_H */
