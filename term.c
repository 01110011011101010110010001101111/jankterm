#include <vte/vte.h>

int FONT_SIZE = 12;

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

    // Ctrl Shift 
    if ((event->state & (GDK_CONTROL_MASK | GDK_SHIFT_MASK)) == (GDK_CONTROL_MASK | GDK_SHIFT_MASK)) {
        switch (event->keyval) {
            case GDK_KEY_c:
                vte_terminal_copy_clipboard_format(terminal, VTE_FORMAT_TEXT);
                return TRUE;
            case GDK_KEY_v:
                vte_terminal_paste_clipboard(terminal);
                return TRUE;
        }
    // Ctrl
    } else if (event->state & GDK_CONTROL_MASK) {
        switch (event->keyval) {
            case GDK_KEY_plus:
                adjust_font_size(terminal, 2);
                return TRUE;
            case GDK_KEY_minus:
                adjust_font_size(terminal, -2);
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

    /* Put widgets together and run the main loop */
    gtk_container_add(GTK_CONTAINER(window), terminal);
    gtk_widget_show_all(window);
    gtk_main();
}
