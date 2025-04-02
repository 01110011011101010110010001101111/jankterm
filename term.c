#include <vte/vte.h>
// #include <cairo.h>
// #include <gdk-pixbuf/gdk-pixbuf.h>
// #include <glib/gstdio.h>

int FONT_SIZE = 12;

// void render_image(const char *image_path, VteTerminal *terminal) {
//     GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(image_path, NULL);
//     if (!pixbuf) {
//         g_printerr("Failed to load image: %s\n", image_path);
//         return;
//     }
// 
//     // Create a Cairo surface from the GdkPixbuf
//     cairo_surface_t *surface = gdk_cairo_surface_create_from_pixbuf(pixbuf, 0, 0);
//     cairo_t *cr = cairo_create(surface);
// 
//     // Get the terminal's window and draw the image
//     GdkWindow *window = gtk_widget_get_window(GTK_WIDGET(terminal));
//     cairo_set_source_surface(cr, surface, 0, 0);
//     cairo_paint(cr);
// 
//     // Clean up
//     cairo_destroy(cr);
//     cairo_surface_destroy(surface);
//     g_object_unref(pixbuf);
// }
// 
// gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data) {
//     VteTerminal *terminal = VTE_TERMINAL(widget);
// 
//     if (event->state & GDK_CONTROL_MASK) {
//         switch (event->keyval) {
// 
//             case GDK_KEY_I: // Let's say Ctrl+I triggers image rendering
//                 render_image("path/to/image.png", terminal);
//                 return TRUE;
//         }
//     }
// 
//     return FALSE;
// }

// void handle_image_escape_sequence(const char *sequence, VteTerminal *terminal) {
//     // Parse the sequence to extract the image path
//     const char *image_path = extract_image_path(sequence);
//     if (image_path) {
//         render_image(image_path, terminal);
//     }
// }

int main();

static void child_ready(VteTerminal *terminal, GPid pid, GError *error, gpointer user_data) {
    if (!terminal) return;
    if (pid == -1) gtk_main_quit();
}

void adjust_font_size(VteTerminal *terminal, int adjustment) {
    FONT_SIZE += adjustment;
    gchar *new_font = g_strdup_printf("Monospace %d", FONT_SIZE);
    vte_terminal_set_font(terminal, pango_font_description_from_string(new_font));
    g_free(new_font);
}

gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data) {
    VteTerminal *terminal = VTE_TERMINAL(widget);

    if (event->state & GDK_CONTROL_MASK) {
        switch (event->keyval) {
            case GDK_KEY_plus:
                adjust_font_size(terminal, 2);
                return TRUE;
            case GDK_KEY_minus:
                adjust_font_size(terminal, -2);
                return TRUE;
            case GDK_KEY_C:
            case GDK_KEY_y:
                vte_terminal_copy_clipboard_format(terminal, VTE_FORMAT_TEXT);
                return TRUE;
            case GDK_KEY_V:
            case GDK_KEY_p:
                vte_terminal_paste_clipboard(terminal);
                return TRUE;
            case GDK_KEY_N:
                main();
                return TRUE;
        }
    }

    return FALSE;
}

gboolean on_context_menu(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    if (event->button == 3) { // Right mouse button
        GtkWidget *menu = gtk_menu_new();

        GtkWidget *copy_item = gtk_menu_item_new_with_label("Copy");
        g_signal_connect(copy_item, "activate", G_CALLBACK(vte_terminal_copy_clipboard_format), VTE_TERMINAL(widget));
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), copy_item);

        GtkWidget *paste_item = gtk_menu_item_new_with_label("Paste");
        g_signal_connect(paste_item, "activate", G_CALLBACK(vte_terminal_paste_clipboard), VTE_TERMINAL(widget));
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), paste_item);

        gtk_widget_show_all(menu);
        gtk_menu_popup_at_pointer(GTK_MENU(menu), (GdkEvent *)event);
        return TRUE;
    }
    return FALSE;
}

int main(int argc, char *argv[]) {
    GtkWidget *window, *terminal;

    /* Initialise GTK, the window and the terminal */
    gtk_init(&argc, &argv);
    terminal = vte_terminal_new();
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "jankterm");

    /* Set custom color scheme */
    GdkRGBA foreground_color = {0.0, 0.0, 0.0, 1.0};
    GdkRGBA background_color = {1.0, 1.0, 1.0, 1.0};
    GdkRGBA palette[16] = {
        {0.9, 0.9, 0.9, 1.0},
        {1.0, 0.0, 0.0, 1.0},
        {0.0, 1.0, 0.0, 1.0},
        {1.0, 1.0, 0.0, 1.0},
        {0.4, 0.5, 0.9, 1.0}, // making into a lighter blue
        {1.0, 0.0, 1.0, 1.0},
        {0.0, 1.0, 1.0, 1.0},
        {0.0, 0.0, 0.0, 1.0},
        {0.5, 0.5, 0.5, 1.0},
        {1.0, 0.5, 0.5, 1.0},
        {0.5, 1.0, 0.5, 1.0},
        {1.0, 1.0, 0.5, 1.0},
        {0.5, 0.5, 1.0, 1.0},
        {1.0, 0.5, 1.0, 1.0},
        {0.5, 1.0, 1.0, 1.0},
        {0.0, 0.0, 0.0, 1.0},
    };

    vte_terminal_set_colors(VTE_TERMINAL(terminal), &background_color, &foreground_color, palette, 16);

    /* Start a new shell */
    gchar **envp = g_get_environ();
    gchar **command = (gchar *[]){g_strdup(g_environ_getenv(envp, "SHELL")), NULL };
    g_strfreev(envp);
    vte_terminal_spawn_async(VTE_TERMINAL(terminal),
        VTE_PTY_DEFAULT,
        NULL,         /* working directory  */
        command,      /* command */
        NULL,         /* environment */
        0,            /* spawn flags */
        NULL, NULL,   /* child setup */
        NULL,         /* child pid */
        -1,           /* timeout */
        NULL,         /* cancellable */
        child_ready,  /* callback */
        NULL);        /* user_data */

    /* Connect some signals */
    g_signal_connect(window, "delete-event", gtk_main_quit, NULL);
    g_signal_connect(terminal, "child-exited", gtk_main_quit, NULL);
    g_signal_connect(terminal, "key-press-event", G_CALLBACK(on_key_press), NULL); // Connect key press event
    g_signal_connect(terminal, "button-press-event", G_CALLBACK(on_context_menu), NULL); // Connect right-click event

    // // Connect to the output signal to handle image escape sequences
    // g_signal_connect(terminal, "child-exited", G_CALLBACK(on_child_exited), NULL); // Example for child exit
    // g_signal_connect(terminal, "terminal-output", G_CALLBACK(on_terminal_output), NULL); // Example for terminal output

    /* Put widgets together and run the main loop */
    gtk_container_add(GTK_CONTAINER(window), terminal);
    gtk_widget_show_all(window);
    gtk_main();
}
