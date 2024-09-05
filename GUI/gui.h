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
    float X;                        //X position calculated by Pi Pico and sensors like mpu6050 
    float Y;                        //Y position calculated by Pi Pico and sensors like mpu6050
    float Latitude;                 //GPS Latitude
    float Latitude_dec;             //GPS latitude decimal value
    uint32_t  Latitude_Direction;   //GPS latitude direction
    float Longitude;                //GPS Longitude
    float Longitude_dec;            //GPS Longitude decimal value
    uint32_t  Longitude_Direction;  //GPS Longitude Direction
}point_t;

typedef struct device_Data_t{
    point_t point[max_Metal_Detection_Points];  //Metal detection points from Pi Pico computations(not GPS)
    uint32_t detections_counter;                //Metal detection counter
    float device_X;                             //Current device X position
    float device_Y;                             //Current device Y position
    bool metal_Detected;                        //true -> metal detected 0 otherwise
}device_Data_t;

typedef struct {
    GtkWidget *drawing_area;
    GtkWidget *gps_label;
} AppData;

/// @brief draw Axes, Drawing box and map legend using Cairo
/// @param cr Cairo data structure pointer
void draw_Axes(cairo_t *cr);

/// @brief draw Axes and detections(detections are refreshing every 100ms)
/// @param widget - drawing area widget(in this case)
/// @param cr - Cairo data structure pointer
/// @param data data transferred to draw(refreshed detection points)
void on_draw(GtkWidget *widget, cairo_t *cr, gpointer data);

/// @brief Containing widgets and signals connection
/// @param app - 
/// @param user_data - 
void activate(GtkApplication* app, gpointer user_data);

/// @brief detection refresh callback and refresh current device GPS position
/// @param user_data 
/// @return true
gboolean refresh_detections(gpointer user_data);

/// @brief Destroy main window
/// @param widget -
/// @param data -
void on_window_destroy(GtkWidget *widget, gpointer data);

/// @brief Update GPS current position data(refresh subtitles in gtk app)
/// @param gps_label -
/// @param text - 
void update_gps_label(GtkWidget *gps_label, const char *text);

/// @brief Save and exit button functionalities
/// @param widget -
/// @param data -
void on_save_and_exit_clicked(GtkWidget *widget, gpointer data);
#endif