#include "gui.h"

static metal_Detections_t metal_Detections = {
    .point = {
        {10, 10},
        {20, 20},
        {30, 30},
        {40, 40},
        {150, 50}
    },
    .detections_counter = 5
};

static void draw_Axes(cairo_t *cr)
{
    cairo_set_line_width(cr, 4);
    cairo_set_source_rgb(cr, 0, 0, 0); // Kolor linii - czarny

    //RECTANGLE
    cairo_rectangle(cr, 10, 10, 1290, 990);

    //X AXIS
    cairo_move_to(cr, 10, 505);
    cairo_line_to(cr, 1300, 505);

    //Y AXIS
    cairo_move_to(cr, 655, 10);
    cairo_line_to(cr, 655, 1000);
    cairo_stroke(cr);

    // AXES NAMES
    cairo_set_font_size(cr, 30);  
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL); 
    cairo_move_to(cr, 1270, 500); // X
    cairo_show_text(cr, "X"); 
    cairo_move_to(cr, 625, 40);  // Y
    cairo_show_text(cr, "Y"); 

    //X Scale and Axis description
    cairo_set_font_size(cr, 15);
    int x_origin = 655;
    int scale_spacing_x = 1290 / 50;

    for (int i = -25; i <= 25; i++) {
        if (i == 0) continue;

        int x = x_origin + i * scale_spacing_x;

        if (i % 5 == 0)
        {
            cairo_move_to(cr, x, 530);
            cairo_line_to(cr, x, 505);
            cairo_stroke(cr);

            cairo_set_font_size(cr, 20);
            cairo_move_to(cr, x - 20, 550);
            cairo_show_text(cr, g_strdup_printf("%d", i * 10));
            cairo_set_font_size(cr, 15);
        }
        else
        {
            cairo_move_to(cr, x, 520);
            cairo_line_to(cr, x, 505);
            cairo_stroke(cr);
        }
    }

    //Y Scale and Axis description
    int y_origin = 505;
    int scale_spacing_y = 990 / 50;

    for (int i = -25; i <= 25; i++) {
        if (i == 0)
            continue;

        int y = y_origin - i * scale_spacing_y;

        if (i % 5 == 0)
        {
            cairo_move_to(cr, 655, y);
            cairo_line_to(cr, 680, y);
            cairo_stroke(cr);

            cairo_set_font_size(cr, 20);
            cairo_move_to(cr, 680, y + 10);
            cairo_show_text(cr, g_strdup_printf("%d", i * 10));
            cairo_set_font_size(cr, 15);
        }

        else
        {
            cairo_move_to(cr, 655, y);
            cairo_line_to(cr, 670, y);
            cairo_stroke(cr);
        }
    }
    cairo_stroke(cr);
}

static void draw_Detections(cairo_t *cr, const metal_Detections_t *detections)
{
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 1);

    int x_origin = 655;
    int y_origin = 505;
    int scale_spacing_x = 1290 / 50;
    int scale_spacing_y = 990 / 50;

    for (uint32_t i = 0; i < detections->detections_counter; i++)
    {

        float x = x_origin + (detections->point[i].X * scale_spacing_x / 10);
        float y = y_origin - (detections->point[i].Y * scale_spacing_y / 10);    
        cairo_arc(cr, x, y, 5, 0, 2 * 3.14); 
        cairo_fill(cr);
    }
}

static void on_draw(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    draw_Axes(cr);
    draw_Detections(cr, &metal_Detections); 
}

void activate(GtkApplication* app, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *drawing_area;

    gtk_init(NULL, NULL);

    // Okno
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Metal Detection Application by Krzysztof Plonka");
    gtk_window_set_default_size(GTK_WINDOW(window), 1920, 1080);
    gtk_window_maximize(GTK_WINDOW(window));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);
  
    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, 1320, 1020);
    gtk_grid_attach(GTK_GRID(grid), drawing_area, 0, 3, 2, 1);
    
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), NULL);

    gtk_widget_show_all(window);
    gtk_main();
}
