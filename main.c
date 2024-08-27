#include <gtk/gtk.h>
#include <cairo.h>
#include <glib.h>  // Dla G_PI

#define MAX_POINTS 100  // Maksymalna liczba punktów

typedef struct {
    double x;
    double y;
} Point;

typedef struct {
    Point points[MAX_POINTS];
    int count;
} PlotData;

static PlotData plot_data = {0};

static void draw_axes(cairo_t *cr, int width, int height) {
    cairo_set_line_width(cr, 2);

    // Rysowanie osi Y po lewej stronie
    cairo_move_to(cr, 40, height - 40);  // Początek osi Y
    cairo_line_to(cr, 40, 20);           // Koniec osi Y (linia pionowa)
    cairo_stroke(cr);

    // Rysowanie osi X na dole
    cairo_move_to(cr, 40, height - 40);  // Początek osi X
    cairo_line_to(cr, width - 20, height - 40); // Koniec osi X (linia pozioma)
    cairo_stroke(cr);

    // Oznaczenia osi X
    for (int i = 0; i <= 10; i++) {
        cairo_move_to(cr, 40 + i * (width - 60) / 10, height - 35);
        cairo_line_to(cr, 40 + i * (width - 60) / 10, height - 45);
        cairo_stroke(cr);

        cairo_move_to(cr, 40 + i * (width - 60) / 10, height - 30);
        cairo_show_text(cr, g_strdup_printf("%d", i));
    }

    // Oznaczenia osi Y
    for (int i = 0; i <= 10; i++) {
        cairo_move_to(cr, 35, height - 40 - i * (height - 60) / 10); // Przesunięcie podziałki w lewo
        cairo_line_to(cr, 45, height - 40 - i * (height - 60) / 10); // Przesunięcie podziałki w prawo
        cairo_stroke(cr);

        cairo_move_to(cr, 10, height - 40 - i * (height - 60) / 10); // Przesunięcie tekstu
        cairo_show_text(cr, g_strdup_printf("%d", i));
    }
}


static void on_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
    cairo_set_font_size(cr, 10);

    int width = gtk_widget_get_allocated_width(widget);
    int height = gtk_widget_get_allocated_height(widget);

    draw_axes(cr, width, height);

    if (plot_data.count > 0) {
        cairo_set_source_rgb(cr, 0, 0, 1); // Niebieski kolor

        for (int i = 0; i < plot_data.count; i++) {
            double x = 40 + plot_data.points[i].x * (width - 60) / 10;
            double y = height - 40 - plot_data.points[i].y * (height - 60) / 10;

            cairo_arc(cr, x, y, 5, 0, 2 * G_PI);
            cairo_fill(cr);
        }
    }
}

static void on_submit(GtkWidget *widget, gpointer data) {
    GtkWidget **entry_widgets = (GtkWidget **)data;
    const gchar *x_text = gtk_entry_get_text(GTK_ENTRY(entry_widgets[0]));
    const gchar *y_text = gtk_entry_get_text(GTK_ENTRY(entry_widgets[1]));

    // Parsowanie danych
    if (plot_data.count < MAX_POINTS) {
        plot_data.points[plot_data.count].x = g_strtod(x_text, NULL);
        plot_data.points[plot_data.count].y = g_strtod(y_text, NULL);
        plot_data.count++;
    }

    // Przerysowanie wykresu
    gtk_widget_queue_draw(gtk_widget_get_parent(widget));
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *x_entry;
    GtkWidget *y_entry;
    GtkWidget *submit_button;
    GtkWidget *drawing_area;

    gtk_init(NULL, NULL);


    // Tworzenie głównego okna
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Wykres punktowy");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Tworzenie siatki
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Tworzenie pól do wprowadzania danych
    x_entry = gtk_entry_new();
    y_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("X:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), x_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Y:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), y_entry, 1, 1, 1, 1);

    // Tworzenie przycisku "Wprowadź dane"
    submit_button = gtk_button_new_with_label("Wprowadź dane");
    gtk_grid_attach(GTK_GRID(grid), submit_button, 0, 2, 2, 1);

    // Tworzenie obszaru rysowania
    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, 600, 300);
    gtk_grid_attach(GTK_GRID(grid), drawing_area, 0, 3, 2, 1);

    // Połączenie sygnałów
    GtkWidget *entries[] = { x_entry, y_entry };
    g_signal_connect(submit_button, "clicked", G_CALLBACK(on_submit), entries);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), NULL);

    // Wyświetlanie wszystkich elementów
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
