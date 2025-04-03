#include <vte/vte.h>

int FONT_SIZE = 12;

// from term.c
int main();

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
                pid_t pid = fork();
                switch (pid) {
                    case -1:
                        // fork failed
                        return FALSE;
                    case 0:
                        main();
                    default:
                        return TRUE;
                }
        }
    }
    return FALSE;
}
