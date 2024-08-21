#include <gtk/gtk.h>

static void on_button_clicked(GtkButton *button, gpointer user_data) {
    GtkLabel *label = GTK_LABEL(user_data);
    gtk_label_set_text(label, "Hello111 , World!");
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button;
    GtkWidget *label;
    //comment

    // Tworzenie nowego okna
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Hello1 World");
    
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    // Tworzenie siatki do układania widgetów
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Tworzenie etykiety
    label = gtk_label_new("Clickme the button");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    // Tworzenie przycisku
    button = gtk_button_new_with_label("Click Me");
    gtk_grid_attach(GTK_GRID(grid), button, 0, 1, 1, 1);

    // Połączenie sygnału kliknięcia przycisku z funkcją obsługi
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), label);

    // Pokazywanie wszystkich widgetów
    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    // Tworzenie instancji aplikacji GTK
    app = gtk_application_new("org.example.helloworld", G_APPLICATION_DEFAULT_FLAGS);

    // Rejestracja funkcji aktywacji aplikacji
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    // Uruchamianie aplikacji
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
