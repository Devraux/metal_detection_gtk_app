#include "gui.h"

static metal_Detections_t metal_Detections = {0};

static void draw_Axes(cairo_t *cr)
{
    cairo_set_line_width(cr, 4);
    cairo_set_source_rgb(cr, 0, 0, 0); //black line color

    // Rectangle Frame
    cairo_rectangle(cr, 10, 10, 1300, 1000);

    // OY
    cairo_move_to(cr, 660, 10);
    cairo_line_to(cr, 660, 1000 + 10);

    // OX
    cairo_move_to(cr, 10, 510);
    cairo_line_to(cr, 1300 + 10, 510);

    // OY Arrow
    cairo_move_to(cr, 660, 10);  
    cairo_line_to(cr, 650, 50);  
    cairo_move_to(cr, 660, 10);
    cairo_line_to(cr, 670, 50);  
    
    // OX Arrow
    cairo_move_to(cr, 1300 + 10, 510);  
    cairo_line_to(cr, 1270, 520);  
    cairo_move_to(cr, 1300 + 10, 510);
    cairo_line_to(cr, 1270, 500);

    // OX and OY Axes Name
    cairo_set_font_size(cr, 30);  
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL); 
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_move_to(cr, 1280, 550);
    cairo_show_text(cr, "X"); 
    cairo_move_to(cr, 675, 40);
    cairo_show_text(cr, "Y"); 


    cairo_stroke(cr);
}

static void on_draw(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    int width = gtk_widget_get_allocated_width(widget);
    int height = gtk_widget_get_allocated_height(widget);

     draw_Axes(cr);
}


void activate(GtkApplication* app, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *drawing_area;

    gtk_init(NULL, NULL);

    //WINDOW
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "Metal Detection Application by Krzysztof Plonka");
    gtk_window_set_default_size(GTK_WINDOW(window), 1920, 1080);
    gtk_window_maximize(GTK_WINDOW(window));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    //WINDOW GRID
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    //WIDNOW Drawing Area
    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, 1320, 1020);
    gtk_grid_attach(GTK_GRID(grid), drawing_area, 0, 3, 2, 1);
    
    //WINDOW Signal Connection
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), NULL);

    gtk_widget_show_all(window);
    gtk_main();
}