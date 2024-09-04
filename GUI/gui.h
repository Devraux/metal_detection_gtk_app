#ifndef _GUI_
#define _GUI_

#include <gtk/gtk.h>
#include <cairo.h>
#include <stdint.h>
#include <math.h>
#include <stdint.h>

#include "../wifi/wifi.h"



#define max_Metal_Detection_Points 300 

typedef struct point_t{
    float X;
    float Y;
}point_t;

typedef struct metal_Detections_t{
    point_t point[max_Metal_Detection_Points];
    uint32_t detections_counter;
}metal_Detections_t;


static void draw_Axes(cairo_t *cr);

static void on_draw(GtkWidget *widget, cairo_t *cr, gpointer data);

void activate(GtkApplication* app, gpointer user_data);

#endif