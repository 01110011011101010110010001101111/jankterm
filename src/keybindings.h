#include <vte/vte.h>

void adjust_font_size(VteTerminal *terminal, int adjustment);

gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data); 
