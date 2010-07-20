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



#include <stdio.h>
#include "callbacks.h"
#include "input.h"

#define SIGNAL_STRENGTH_HEIGHTS 256
#define SCALE_GRID 20
#define DESC_LABEL_SIZE 2000

static int desc_label_changed;
static int desc_mode_state;
enum desc_mode { DESC_INFO, DESC_DEBUG, DESC_STATS };

static int quit_exit = 0;

/* pixmap sizes for each piece of the display */
static int rows_x, rows_y, cols_x, cols_y, midd_x, midd_y;

static GtkAdjustment *row_threshold_adjustment, *column_threshold_adjustment;
static GtkScale *row_threshold_scale, *column_threshold_scale;
static GtkCheckButton *threshold_lock_checkbutton;
static GtkLabel *dt_description_label;
static GtkAboutDialog *dt_test_about_dialog;
static GtkEventBox *label_eventbox;

static GdkPixmap *columns_pixmap = NULL,
  *rows_pixmap = NULL, *middle_pixmap = NULL;

static GtkWidget *w_cols = NULL, *w_rows = NULL, *w_midd = NULL;


/** define display colors **/
const char *user_color_strings[MAX_INPUTS] = {
  "magenta", "cyan", "green", "yellow" };

const char *scale_color_string = "gray25";
const char *threshold_color_string = "gray75";

GdkColor black_color = { 0x0000, 0x0000, 0x0000, 0x0000 };
GdkColor white_color = { 0x0000, 0xffff, 0xffff, 0xffff };

static GdkGC *pens[MAX_INPUTS];
static GdkGC *scale_pen = NULL;
static GdkGC *threshold_pen = NULL;

GdkGC *get_pen(GdkPixmap *pixmap, const char *color_string) {
  GdkGC *gc;
  GdkColor color;
  gc = gdk_gc_new(pixmap);

  gdk_color_parse(color_string, &color);
  gdk_colormap_alloc_color(gdk_colormap_get_system(),
			   &color, FALSE, FALSE);

  gdk_gc_set_foreground(gc, &color);
  gdk_gc_set_fill(gc, GDK_SOLID);
  return gc;
}

void setup_widgets(GladeXML *xml) {
  int i;

  w_rows = glade_xml_get_widget(xml, "rows_drawingarea");
  w_cols = glade_xml_get_widget(xml, "columns_drawingarea");
  w_midd = glade_xml_get_widget(xml, "middle_drawingarea");

  row_threshold_scale =
    (GtkScale *) glade_xml_get_widget(xml, "row_threshold_scale");
  column_threshold_scale =
    (GtkScale *) glade_xml_get_widget(xml, "column_threshold_scale");

  row_threshold_adjustment =
    gtk_range_get_adjustment((GtkRange *) row_threshold_scale);
  g_object_ref(row_threshold_adjustment);
  column_threshold_adjustment =
    gtk_range_get_adjustment((GtkRange *) column_threshold_scale);

  gtk_adjustment_set_value(row_threshold_adjustment,
			   (gdouble) ROW_THRESHOLD);
  gtk_adjustment_set_value(column_threshold_adjustment,
			   (gdouble) COL_THRESHOLD);

  /* set the threshold sliders lock checkbutton to active and lock the
     sliders together */
  threshold_lock_checkbutton = (GtkCheckButton *)
    glade_xml_get_widget(xml, "threshold_lock_checkbutton");
  gtk_toggle_button_set_active((GtkToggleButton *) threshold_lock_checkbutton,
			       TRUE);
  gtk_range_set_adjustment((GtkRange *) row_threshold_scale,
			   column_threshold_adjustment);

  dt_description_label = (GtkLabel *)
    glade_xml_get_widget(xml, "dt_description_label");
		       
  desc_label_changed = 1;
  desc_mode_state = DESC_INFO;

  gtk_widget_modify_fg((GtkWidget *) dt_description_label, GTK_STATE_NORMAL,
		       &white_color);

  label_eventbox = (GtkEventBox *) glade_xml_get_widget(xml, "label_eventbox");
  gtk_widget_modify_bg((GtkWidget *) label_eventbox, GTK_STATE_NORMAL,
		       &black_color);

  dt_test_about_dialog = (GtkAboutDialog*)
    glade_xml_get_widget(xml, "dt_test_aboutdialog");

  rows_x = SIGNAL_STRENGTH_HEIGHTS;
  rows_y = dt.num_rows * dt.num_users;
  cols_x = dt.num_columns * dt.num_users;
  cols_y = SIGNAL_STRENGTH_HEIGHTS;
  midd_x = dt.num_columns * dt.num_users;
  midd_y = dt.num_rows * dt.num_users;

  gtk_widget_set_size_request(w_rows, rows_x, rows_y);
  gtk_widget_set_size_request(w_cols, cols_x, cols_y);
  gtk_widget_set_size_request(w_midd, midd_x, midd_y);
  gtk_widget_set_size_request((GtkWidget *) dt_description_label,
			      rows_x, cols_y);

  if (middle_pixmap) gdk_pixmap_unref(middle_pixmap);
  middle_pixmap = gdk_pixmap_new(w_midd->window, midd_x, midd_y, -1);
  gdk_draw_rectangle(middle_pixmap, w_midd->style->black_gc, TRUE,
		     0, 0, midd_x, midd_y);

  if (rows_pixmap) gdk_pixmap_unref(rows_pixmap);
  rows_pixmap = gdk_pixmap_new(w_rows->window, rows_x, rows_y, -1);
  gdk_draw_rectangle(rows_pixmap, w_rows->style->black_gc, TRUE,
		     0, 0, rows_x, rows_y);

  if (columns_pixmap) gdk_pixmap_unref(columns_pixmap);
  columns_pixmap = gdk_pixmap_new(w_cols->window, cols_x, cols_y, -1);
  gdk_draw_rectangle(columns_pixmap, w_cols->style->black_gc, TRUE,
		     0, 0, cols_x, cols_y);

  /* allocate GCs for our colors */
  for (i = 0; i < MAX_INPUTS; i++)
    pens[i] = get_pen(middle_pixmap, user_color_strings[i]);

  scale_pen = get_pen(middle_pixmap, scale_color_string);
  threshold_pen = get_pen(middle_pixmap, threshold_color_string);
}


char desc_label_text[DESC_LABEL_SIZE];

void set_description_label() {
  int i, r;

  if (desc_label_changed || desc_mode_state == DESC_DEBUG
      || desc_mode_state == DESC_STATS) {

    switch (desc_mode_state) {
    case DESC_DEBUG:
      r = 0;
      r += sprintf(desc_label_text + r, "<tt>");
      r += sprintf(desc_label_text + r,
		   "DT: %10u prg: %10d\nMisses: %d Largest miss: %d\n",
		   *seq, is[0].iterations, seq_misses, largest_seq_diff);
      r += sprintf(desc_label_text + r, "</tt>");
      break;

    case DESC_STATS:
      r = 0;
      r += sprintf(desc_label_text + r, "<tt>"
		   "       <b><u>Rows</u></b>     |    <b><u>Columns</u></b>  \n"
		   "   min mean max | min mean max\n");
      for (i = 0; i < dt.num_users; i++) {
	r += sprintf(desc_label_text + r,
		     "<span foreground=\"%s\">"
		     "%d: %3d  %3d %3d | %3d  %3d %3d\n"
		     "</span>",
		     user_color_strings[i], i,
		     is[i].row_sig_min, is[i].row_sig_mean, is[i].row_sig_max,
	is[i].col_sig_min, is[i].col_sig_mean, is[i].col_sig_max);
      }
      if (dt.shield_drv_on) {
	for (i = 0; i < dt.num_users; i++) {
	  r += sprintf(desc_label_text + r,
		       "<span foreground=\"%s\">"
		       "User %d shield drive: %d\n</span>",
		       user_color_strings[i], i, *(is[i].sdrv));
	}
      }
      r += sprintf(desc_label_text + r, "</tt>");
      break;

    case DESC_INFO:
    default:
      /* set the text in the description label */
      r = 0;
      r += sprintf(desc_label_text + r, "<tt>");
      r += sprintf(desc_label_text + r, "dev: %s\n"
	      "Serial number: %s%.7s\n"
	      "Columns: %d Rows: %d Users: %d\n"
	      "<span foreground=\"%s\">User 0 </span>"
	      "<span foreground=\"%s\">User 1 </span>"
	      "<span foreground=\"%s\">User 2 </span>"
	      "<span foreground=\"%s\">User 3\n\n</span>"
	      "<b><u>Thresholds:</u></b> Column %d  Row %d\n",
	      dt_device_name,
	      dt_types[eebuff[DT_EE_UNIQUE_SERIALNUM_0]],
	      eebuff + DT_EE_UNIQUE_SERIALNUM_1,
	      dt.num_columns, dt.num_rows, dt.num_users,
	      user_color_strings[0], user_color_strings[1],
	      user_color_strings[2], user_color_strings[3],
	      col_threshold, row_threshold);
      r += sprintf(desc_label_text + r, "\nShield drive is: ");
      if (dt.shield_drv_on)
	r += sprintf(desc_label_text + r, "<b>ACTIVE</b>\n");
      else
	r += sprintf(desc_label_text + r, "<b>NOT</b> active\n");

      r += sprintf(desc_label_text + r, "</tt>");
      break;
    }

    gtk_label_set_markup(dt_description_label, desc_label_text);
    desc_label_changed = 0;
  }
}

G_MODULE_EXPORT gboolean
on_middle_drawingarea_expose_event(GtkWidget *widget, GdkEventExpose *event,
				   gpointer user_data) {
  gdk_draw_pixmap(widget->window,
		  widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
		  middle_pixmap,
		  event->area.x, event->area.y,
		  event->area.x, event->area.y,
		  event->area.width, event->area.height);

  return FALSE;
}

G_MODULE_EXPORT gboolean
on_rows_drawingarea_expose_event(GtkWidget *widget,
				 GdkEventExpose *event, gpointer user_data) {
  gdk_draw_pixmap(widget->window,
		  widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
		  rows_pixmap,
		  event->area.x, event->area.y,
		  event->area.x, event->area.y,
		  event->area.width, event->area.height);

  return FALSE;
}

G_MODULE_EXPORT gboolean
on_columns_drawingarea_expose_event(GtkWidget *widget, GdkEventExpose *event,
				    gpointer user_data) {
  gdk_draw_pixmap(widget->window,
		  widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
		  columns_pixmap,
		  event->area.x, event->area.y,
		  event->area.x, event->area.y,
		  event->area.width, event->area.height);

  return FALSE;
}


G_MODULE_EXPORT void
on_row_threshold_scale_value_changed(GtkRange *range,
				     gpointer user_data) {
  row_threshold = (int)
    gtk_adjustment_get_value(gtk_range_get_adjustment(range));
  desc_label_changed = 1;
}

G_MODULE_EXPORT void
on_column_threshold_scale_value_changed(GtkRange *range,
					gpointer user_data) {
  col_threshold = (int)
    gtk_adjustment_get_value(gtk_range_get_adjustment(range));
  desc_label_changed = 1;
}

G_MODULE_EXPORT void
on_threshold_lock_checkbutton_toggled(GtkToggleButton *togglebutton,
				      gpointer user_data) {
  if (gtk_toggle_button_get_active(togglebutton)) {
    /* row and col threshold sliders locked together */
    gtk_range_set_adjustment((GtkRange *) row_threshold_scale,
			     column_threshold_adjustment);
    gtk_adjustment_value_changed(column_threshold_adjustment);
  }
  else {
    /* row and col threshold sliders unlocked and independent */
    gtk_range_set_adjustment((GtkRange *) row_threshold_scale,
			     row_threshold_adjustment);
    gtk_adjustment_set_value
      (row_threshold_adjustment,
       gtk_adjustment_get_value(column_threshold_adjustment));
  }
}

G_MODULE_EXPORT void
on_info_button_clicked(GtkButton* button, gpointer user_data) {
  desc_mode_state = DESC_INFO;
  desc_label_changed = 1;
}
G_MODULE_EXPORT void
on_debug_button_clicked(GtkButton* button, gpointer user_data) {
  desc_mode_state = DESC_DEBUG;
  desc_label_changed = 1;
}
G_MODULE_EXPORT void
on_stats_button_clicked(GtkButton* button, gpointer user_data) {
  desc_mode_state = DESC_STATS;
  desc_label_changed = 1;
}
G_MODULE_EXPORT void
on_about_button_clicked(GtkButton* button, gpointer user_data) {
  gtk_widget_show_all((GtkWidget *) dt_test_about_dialog);
}



#define CROSS_RADIUS (20)

/* "is" is the input set for the associated user */
void display_data(struct input_set *is) {
  int i;
  int xx, yy;
  int bbxmin, bbxmax, bbymin, bbymax;

  /* draw 2D (middle) stuff */

  if (is->maxcol >= 0 && is->maxrow >= 0) {
    if (dt.columns_left_to_right) {
      xx = dt.num_users * is->maxcol;
      bbxmax = is->bbcolmax * dt.num_users;
      bbxmin = is->bbcolmin * dt.num_users;
    }
    else {
      xx = dt.num_users * (dt.num_columns - is->maxcol - 1);
      bbxmax = (dt.num_columns - is->bbcolmin - 1) * dt.num_users;
      bbxmin = (dt.num_columns - is->bbcolmax - 1) * dt.num_users;
    }
    
    if (dt.rows_top_to_bottom) {
      yy = dt.num_users * is->maxrow;
      bbymax = is->bbrowmax * dt.num_users;
      bbymin = is->bbrowmin * dt.num_users;
    }
    else {
      yy = dt.num_users * (dt.num_rows - is->maxrow - 1);
      bbymax = (dt.num_rows - is->bbrowmin - 1) * dt.num_users;
      bbymin = (dt.num_rows - is->bbrowmax - 1) * dt.num_users;
    }

    gdk_draw_line(middle_pixmap, pens[is->number],
		  xx, yy - CROSS_RADIUS, xx, yy + CROSS_RADIUS);
    gdk_draw_line(middle_pixmap, pens[is->number],
		  xx - CROSS_RADIUS, yy, xx + CROSS_RADIUS, yy);

    gdk_draw_rectangle(middle_pixmap, pens[is->number],
		       FALSE,
		       bbxmin, bbymin,
		       bbxmax - bbxmin, bbymax - bbymin);
  }

  /*** draw the row and column histograms ***/

  /* columns */
  for (i = 0; i < dt.num_columns; i++) {
    if (dt.columns_left_to_right)
      gdk_draw_rectangle(columns_pixmap, pens[is->number], TRUE,
			 dt.num_users * i + is->number, 0,
			 1, is->cols[i]);
    else
      gdk_draw_rectangle(columns_pixmap, pens[is->number], TRUE,
			 dt.num_users * (dt.num_columns - i - 1) + is->number,
			 0,
			 1, is->cols[i]);
  }


  /* rows */
  for (i = 0; i < dt.num_rows; i++) {
    if (dt.rows_top_to_bottom)
      gdk_draw_rectangle(rows_pixmap, pens[is->number], TRUE,
			 0, dt.num_users * i + is->number,
			 is->rows[i], 1);
    else
      gdk_draw_rectangle(rows_pixmap, pens[is->number], TRUE,
			 0, dt.num_users * (dt.num_rows - i - 1) + is->number,
			 is->rows[i], 1);
  }
}


/* draw the offscreen pixmaps to the screen and then erase them */
void recycle_display() {
  int i;
  GdkRectangle ur;

  if (quit_exit != 0) {
    return;
  }

  /* set the text in the description label */
  set_description_label();

  /* draw the pixmaps to the screen */

  /* middle pixmap */
  ur.x = ur.y = 0;
  ur.width = w_midd->allocation.width;
  ur.height = w_midd->allocation.height;
  gtk_widget_draw(w_midd, &ur);

  /* column pixmap w/ grid and threshold */
  for (i = SCALE_GRID; i < SIGNAL_STRENGTH_HEIGHTS; i += SCALE_GRID)
    gdk_draw_rectangle(columns_pixmap, scale_pen, TRUE,
		       0, i, cols_x, 1);
  gdk_draw_rectangle(columns_pixmap, threshold_pen, TRUE,
		     0, col_threshold, cols_x, 1);
  ur.x = ur.y = 0;
  ur.width = w_cols->allocation.width;
  ur.height = w_cols->allocation.height;
  gtk_widget_draw(w_cols, &ur);

  /* row pixmap w/ grid and threshold */
  for (i = SCALE_GRID; i < SIGNAL_STRENGTH_HEIGHTS; i += SCALE_GRID)
    gdk_draw_rectangle(rows_pixmap, scale_pen, TRUE,
		       i, 0, 1, cols_x);
  gdk_draw_rectangle(rows_pixmap, threshold_pen, TRUE,
		     row_threshold, 0, 1, cols_x);
  ur.x = ur.y = 0;
  ur.width = w_rows->allocation.width;
  ur.height = w_rows->allocation.height;
  gtk_widget_draw(w_rows, &ur);

  /* erase the pixmaps */

  /* middle */
  gdk_draw_rectangle(middle_pixmap, w_midd->style->black_gc,
		     TRUE, 0, 0,
		     w_midd->allocation.width,
		     w_midd->allocation.height);
  
  /* columns */
  gdk_draw_rectangle(columns_pixmap, w_cols->style->black_gc,
		     TRUE, 0, 0,
		     w_cols->allocation.width,
		     w_cols->allocation.height);

  /* rows */
  gdk_draw_rectangle(rows_pixmap, w_rows->style->black_gc,
		     TRUE, 0, 0,
		     w_rows->allocation.width,
		     w_rows->allocation.height);
}
