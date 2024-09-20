#include "gui.h"

pico_To_Server_Frame_t pico_To_Server_Data = {0};

device_Data_t device_data = {0};

guint timeout_id = 0;
 
void draw_Axes(cairo_t *cr)
{
    cairo_set_line_width(cr, 4);
    cairo_set_source_rgb(cr, 0, 0, 0); 

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
    cairo_move_to(cr, 1215, 490); // X
    cairo_show_text(cr, "X[cm]"); 
    cairo_move_to(cr, 570, 40);  // Y
    cairo_show_text(cr, "Y[cm]"); 

    // Plot Legend Subtitles
    cairo_set_font_size(cr, 23);  
    cairo_select_font_face(cr, "Impact", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL); 
    cairo_move_to(cr, 1140, 955); 
    cairo_show_text(cr, " - Vehicle pos."); 
    cairo_move_to(cr, 1140, 985);  
    cairo_show_text(cr, " - Detection pos."); 
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 1);

    cairo_arc(cr, 1135, 977, 5, 0, 2 * 3.14); 
    cairo_fill(cr);
    cairo_set_source_rgb(cr, 1, 0, 0);
    cairo_arc(cr, 1135, 947, 5, 0, 2 * 3.14); 
    cairo_fill(cr);
    cairo_set_line_width(cr, 4);
    cairo_set_source_rgb(cr, 0, 0, 0);


    //X Scale and Axis description
    cairo_set_font_size(cr, 15);
    int x_origin = 655;
    int scale_spacing_x = 1290 / 50;

    for (int i = -25; i <= 25; i++) {
        if (i == 0) continue;

        int x = x_origin + i * scale_spacing_x;
        // X Axis gray spacing lines
        cairo_set_source_rgba(cr, 0, 0, 0, 0.2);
        cairo_set_line_width(cr, 2);
        cairo_move_to(cr, x, 10);
        cairo_line_to(cr, x, 1000);
        cairo_stroke(cr);
        cairo_set_source_rgba(cr, 0, 0, 0, 1);
        cairo_set_line_width(cr, 4);

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

        // Y Axis gray spacing lines
        cairo_set_source_rgba(cr, 0, 0, 0, 0.2);
        cairo_set_line_width(cr, 2);
        cairo_move_to(cr, 10, y);
        cairo_line_to(cr, 1300,  y);
        cairo_stroke(cr);
        cairo_set_source_rgba(cr, 0, 0, 0, 1);
        cairo_set_line_width(cr, 4);

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

void draw_Detections(cairo_t *cr, const device_Data_t *device_Data)
{
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 1);

    int x_origin = 655;
    int y_origin = 505;
    int scale_spacing_x = 1290 / 50;
    int scale_spacing_y = 990 / 50;

    for (uint32_t i = 0; i < device_Data->detections_counter; i++)
    {

        float x = x_origin + ((device_Data->point[i].X * 100.0f) * scale_spacing_x / 10.0f);
        float y = y_origin - ((device_Data->point[i].Y * 100.0f) * scale_spacing_y / 10.0f);    
        cairo_arc(cr, x, y, 5, 0, 2 * 3.14); 
        cairo_fill(cr);
    }

        //device position based on mpu6050 sensor
        cairo_set_source_rgb(cr, 1, 0, 0); //Red colour(red dot on drawing area)
        float device_x = x_origin + ((device_Data->device_X * 100.0f) * scale_spacing_x / 10.0f);
        float device_y = y_origin - ((device_Data->device_Y * 100.0f) * scale_spacing_y / 10.0f);    
        cairo_arc(cr, device_x, device_y, 10, 0, 2 * 3.14); 
        cairo_fill(cr);
        cairo_set_source_rgb(cr, 0, 0, 0);

}

void on_draw(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    draw_Axes(cr);
    draw_Detections(cr, &device_data); 
}

void activate(GtkApplication* app, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *drawing_area;
    GtkWidget *GPS_label;
    GtkWidget *save_exit_button;
    GtkWidget *scanning_status_label;
    GtkWidget *connection_status_label;

    ///CSS Loading from file <-> ../style.css
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "../style.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_init(NULL, NULL);

    ///Window Initialization
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Metal Detection Application by Krzysztof Plonka");
    gtk_window_set_default_size(GTK_WINDOW(window), 1920, 1080);
    gtk_window_maximize(GTK_WINDOW(window));
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    /// Grid Initialization
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    /// Drawing area Initialization
    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, 1320, 1020);
    gtk_grid_attach(GTK_GRID(grid), drawing_area, 1, 1, 20, 20);

    /// GPS current data label
    GPS_label = gtk_label_new("GPS device current pos.");
    gtk_grid_attach(GTK_GRID(grid), GPS_label, 23, 1, 1, 1);

    ///scanning status label
    scanning_status_label = gtk_label_new("Scanning status: OK");
    gtk_grid_attach(GTK_GRID(grid), scanning_status_label, 23, 2, 1, 1);

    ///connection status label
    connection_status_label = gtk_label_new("Connection status: OK");
    gtk_grid_attach(GTK_GRID(grid), connection_status_label, 23, 4, 1, 1);

    /// Save and Exit button
    save_exit_button = gtk_button_new_with_label("Save and Exit");
    gtk_grid_attach(GTK_GRID(grid), save_exit_button, 23, 6, 1, 1);
    g_signal_connect(save_exit_button, "clicked", G_CALLBACK(on_save_and_exit_clicked), NULL);

    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), NULL);

    AppData *data = g_new(AppData, 1);
    data->drawing_area = drawing_area;
    data->gps_label = GPS_label;
    data->scanning_status_label = scanning_status_label;
    data->connection_status_label = connection_status_label;

    gtk_widget_show_all(window);
    g_timeout_add(100, refresh_detections, data);
    gtk_main();
}

gboolean refresh_detections(gpointer user_data)
{
    /// Input data casting and pointers assigns
    AppData *data = (AppData *)user_data;
    GtkWidget *drawing_area = data->drawing_area;
    GtkWidget *gps_label = data->gps_label;
    GtkWidget *scanning_status_label = data->scanning_status_label;
    GtkWidget *connection_status_label = data->connection_status_label;

    /// Try remove data from Queue(receive buffer)
    queue_try_remove(&pico_To_Server_Queue, &pico_To_Server_Data);
    device_data.metal_Detected = pico_To_Server_Data.metal_Detection;
    device_data.device_X = pico_To_Server_Data.MPU_X;
    device_data.device_Y = pico_To_Server_Data.MPU_Y;

    if (device_data.metal_Detected == true) {
        device_data.point[device_data.detections_counter].X = device_data.device_X;
        device_data.point[device_data.detections_counter].Y = device_data.device_Y;

        device_data.point->Latitude = pico_To_Server_Data.GPS_Latitude;
        device_data.point->Latitude_dec = pico_To_Server_Data.GPS_Latitude_dec;
        device_data.point->Latitude_Direction = pico_To_Server_Data.GPS_Latitude_Direction;
        device_data.point->Longitude = pico_To_Server_Data.GPS_Longitude;
        device_data.point->Longitude_dec = pico_To_Server_Data.GPS_Longitude_dec;
        device_data.point->Longitude_Direction = pico_To_Server_Data.GPS_Longitude_Direction;

        device_data.detections_counter++;
    }

    if(device_data.device_X <= 2.5f && device_data.device_X >= -2.5f && device_data.device_Y <= 2.5f && device_data.device_Y >= -2.5f)
    {
        gtk_label_set_text(GTK_LABEL(scanning_status_label), "Scanning status: OK");
        GtkStyleContext *context = gtk_widget_get_style_context(scanning_status_label);
        gtk_style_context_remove_class(context, "scanning-status-out-of-range");
        gtk_style_context_add_class(context, "scanning-status-ok");
    }
    else if(device_data.device_X >= 2.5f || device_data.device_X <= -2.5f || device_data.device_Y >= 2.5f || device_data.device_Y <= -2.5f)
    {   
        gtk_label_set_text(GTK_LABEL(scanning_status_label), "Scanning status: Out of Range");
        GtkStyleContext *context = gtk_widget_get_style_context(scanning_status_label);
        gtk_style_context_remove_class(context, "scanning-status-ok");
        gtk_style_context_add_class(context, "scanning-status-out-of-range");        
    }


    if(pico_IP_Detected == true)
    {
        gtk_label_set_text(GTK_LABEL(connection_status_label), "Connection status: OK, IP addr.:" Pico_Ip_Address);
        GtkStyleContext *context = gtk_widget_get_style_context(connection_status_label);
        gtk_style_context_remove_class(context, "connection-status-device-not-found");
        gtk_style_context_add_class(context, "connection-status-ok");
    }
    else if(pico_IP_Detected == false)
    {   
        gtk_label_set_text(GTK_LABEL(connection_status_label), "Connection status: device not found");
        GtkStyleContext *context = gtk_widget_get_style_context(connection_status_label);
        gtk_style_context_remove_class(context, "connection-status-ok");
        gtk_style_context_add_class(context, "connection-status-device-not-found");        
    } 



    ///// Device status label
    //if(device_data.device_X >= 250 || device_data.device_X <= -250 || device_data.device_Y > 250 || device_data.device_Y <= -250)
    //    gtk_label_set_text(GTK_LABEL(status_label), "Status: Metal Detected");
    //else    
    //    gtk_label_set_text(GTK_LABEL(status_label), "Status: OK");

    ///Print GPS received data
    char *device_data_text_box = g_strdup_printf(
        "GPS device current position. :\nLatitude: %d°%d' %c\nLongitude: %d°%d' %c\n\nDevice XY pos. :\nX: %.2f[cm], Y: %.2f[cm]\n",
        pico_To_Server_Data.GPS_Latitude,
        pico_To_Server_Data.GPS_Latitude_dec,
        INT_To_ASCII(pico_To_Server_Data.GPS_Latitude_Direction),
        pico_To_Server_Data.GPS_Longitude,
        pico_To_Server_Data.GPS_Longitude_dec,
        INT_To_ASCII(pico_To_Server_Data.GPS_Longitude_Direction),
        device_data.device_X * 100.0f,
        device_data.device_Y * 100.0f
    );
    gtk_label_set_text(GTK_LABEL(gps_label), device_data_text_box);
    g_free(device_data_text_box);
    gtk_widget_queue_draw(drawing_area);

    return true;
}

void on_window_destroy(GtkWidget *widget, gpointer data)
{
    if (timeout_id > 0) {
        g_source_remove(timeout_id);  
        timeout_id = 0;  
    }

    gtk_main_quit();
}

void on_save_and_exit_clicked(GtkWidget *widget, gpointer data) {
    FILE *file = fopen("detections.txt", "w");
    if (file == NULL) {
        g_print("Error opening file for writing.\n");
        return;
    }
    fprintf(file, "Welcome to the metal detection vehicle application !!!\n" 
                   "Below is a list of potential metal occurrences recorded by your device.\n" 
                   "Attention!!!\n"
                   "X, Y data is only for reference and may differ significantly from the actual location of the metal object,\n"
                   "therefore it is recommended to use GPS data for a more accurate estimate of the location of the metal occurrence.\n"
                   "Thank you for using my application.\n\n");
    for (uint32_t i = 0; i < device_data.detections_counter; i++)
    {
        fprintf(file, "Detection %d:\n", i + 1);
        fprintf(file, "  X: %.2f\n", device_data.point[i].X);
        fprintf(file, "  Y: %.2f\n", device_data.point[i].Y);
        fprintf(file, "  Latitude: %.2f°%d' %c\n", device_data.point[i].Latitude, (int)device_data.point[i].Latitude_dec, device_data.point[i].Latitude_Direction == 0 ? 'N' : 'S');
        fprintf(file, "  Longitude: %.2f°%d' %c\n", device_data.point[i].Longitude, (int)device_data.point[i].Longitude_dec, device_data.point[i].Longitude_Direction == 0 ? 'E' : 'W');
        fprintf(file, "\n");
    }

    fclose(file);
    gtk_main_quit();
}