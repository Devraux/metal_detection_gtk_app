#ifndef _GUI_
#define _GUI_

#include <gtk/gtk.h>
#include <cairo.h>
#include <stdint.h>
#include <math.h>
#include <stdint.h>
#include <winsock2.h>
#include <windows.h>

#include "../wifi/wifi.h"



#define max_Metal_Detection_Points 300 

typedef struct point_t{
    float X;
    float Y;
}point_t;

typedef struct device_Data_t{
    point_t point[max_Metal_Detection_Points];  //Metal detection points from Pi Pico computations(not GPS)
    uint32_t detections_counter;                //Metal detection counter
    float device_X;                             //Current device X position
    float device_Y;                             //Current device Y position
    bool metal_Detected;                       //true -> metal detected 0 otherwise
}device_Data_t;


void draw_Axes(cairo_t *cr);

void on_draw(GtkWidget *widget, cairo_t *cr, gpointer data);

void activate(GtkApplication* app, gpointer user_data);


gboolean refresh_detections(gpointer user_data);
#endif