#include <gtk/gtk.h>
#include "GUI/gui.h"
#include "wifi/wifi.h"
#include <pthread.h>

int main (int argc, char **argv)
{
    //wifi_Receive_Init();
    wifi_Send_Init();

    GtkApplication *app;
    int status;

    //app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    //g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    //status = g_application_run (G_APPLICATION (app), argc, argv);
    //g_object_unref (app);

    return status;
}