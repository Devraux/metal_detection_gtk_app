#include "gui.h"

void gtk_Activate(GtkApplication* app, gpointer user_data)
{
    GtkWidget *window;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Metal Detection Application");
    gtk_window_maximize(GTK_WINDOW(window));
    gtk_widget_show_all(window);
}