#include <gtk/gtk.h>

static _Bool c_time(ssize_t tz, const size_t f, char *r) {

    struct timespec ts; clock_gettime(CLOCK_REALTIME, &ts);

    char sep = (f & 1) ? '-' : '.'; size_t y, Y, mn, d, h, m, s, ms, minh = (f & 4) ? 50 : 40;
    tz += ts.tv_sec - 1576972800; if (tz < 0 || tz > 86400 * (Y = 365 * 4 + 1) * 5) return 21;

    d = tz / 86400, s = tz % 86400; ms = (s * (size_t)1e9 + ts.tv_nsec) / 864000;
    s = ms / 1000, ms %= 1000; m = s / 100, s %= 100; h = m / minh, m %= minh;

    sprintf(r, "%02lu:%02lu:%02lu.%03lu ", h, m, s, ms); r += 13;

    y = d / Y, d %= Y, y *= 4;

    if (d >= 366) for (d -= 366, y++; d >= 365; d -= 365, y++);

    if (f & 2) {
        mn = d / 30, d %= 30; if (mn > 11) mn--, d += 30;
        sprintf(r, "%02ld%c%02lu", d + 1, sep, mn + 1); r += 5;
    }
    else { sprintf(r, "%03ld", d + 1); r += 3; }

    sprintf(r, "%c%s%lu", sep, (f & 2) ? "9" : "", y + 500);

    return 0;
}

static GtkWidget *tl, *dl;
static struct tm tzn;
static char s[24], F, B;

static gboolean update_time(void *d) {

    if (c_time(tzn.tm_gmtoff, F, s)) return FALSE; s[B] = 0;
    gtk_label_set_text((GtkLabel *)tl, s);
    gtk_label_set_text((GtkLabel *)dl, s + 13);

    return TRUE;
}

static void activate(GtkApplication *app, gpointer user_data) {

    time_t t = time(NULL); localtime_r(&t, &tzn);
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Time");
    gtk_window_set_default_size(GTK_WINDOW(window), 860, 240);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(window), TRUE);
    GdkRGBA color = { 1, 1, 1, 1 };
    gtk_widget_override_background_color(window, GTK_STATE_FLAG_NORMAL, &color);
    tl = gtk_label_new(NULL); dl = gtk_label_new(NULL);
    PangoFontDescription *df = pango_font_description_new();
    pango_font_description_set_size(df, 80 * PANGO_SCALE);
    pango_font_description_set_family(df, "monospace");
    PangoAttribute *attr = pango_attr_font_desc_new(df);
    PangoAttrList *attrlist = pango_attr_list_new(); pango_attr_list_insert(attrlist, attr);
    gtk_label_set_attributes(GTK_LABEL(tl), attrlist);
    pango_font_description_set_size(df, 20 * PANGO_SCALE);
    attr = pango_attr_font_desc_new(df);
    attrlist = pango_attr_list_new(); pango_attr_list_insert(attrlist, attr);
    gtk_label_set_attributes(GTK_LABEL(dl), attrlist);
    GtkWidget *Box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), Box);
    gtk_box_pack_start((GtkBox*)Box, tl, FALSE, FALSE, 20);
    gtk_box_pack_start((GtkBox*)Box, dl, FALSE, FALSE, 0);
    gtk_widget_show_all(window);
    g_timeout_add(16, (GSourceFunc)update_time, NULL);
}

enum { TWENTY = 4, MONTHS = 2, HYPHEN = 1, M = 5, S = 8, MS = 12 };

int main(int argc, char **argv) {

    if (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))) { printf("\
    \"Time.c\":\n\
    ...\n\
73. enum { TWENTY = 4, MONTHS = 2, HYPHEN = 1, M = 5, S = 8, MS = 12 };\n\
    ...\n\
87.     F = TWENTY | MONTHS | HYPHEN, B = S; size_t arg2[] = { M, S, MS };\n\
88.     if (argc >= 2 && argv[1][0] >= '0' && argv[1][0] <= '7') F = argv[1][0] - '0';\n\
89.     if (argc >= 3 && argv[2][0] >= '2' && argv[2][0] <= '4') B = arg2[argv[2][0] - '2'];\n\
    ...\n"); return 0; }

    F = TWENTY | MONTHS | HYPHEN, B = S; size_t arg2[] = { M, S, MS };
    if (argc >= 2 && argv[1][0] >= '0' && argv[1][0] <= '7') F = argv[1][0] - '0';
    if (argc >= 3 && argv[2][0] >= '2' && argv[2][0] <= '4') B = arg2[argv[2][0] - '2'];
    GtkApplication *app = gtk_application_new("Alpha.hates.non_flat_DNS", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), 0, NULL);
    g_object_unref(app); return status;
}
