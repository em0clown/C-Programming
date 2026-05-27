#pragma once
#include <gtk/gtk.h>
#include "game_state.h"

class GameView {
public:
    static GameState* state;
    static GtkWidget *drawingArea;
    static GtkWidget *statusLabel;
    static GtkWidget *moneyLabel;
    static cairo_surface_t *background;
    static int bgWidth;
    static int bgHeight;

    static void init(GameState* gameState);
    static void updateUI();
    static void drawCard(cairo_t *cr, double x, double y, double w, double h, Card card, bool faceUp);
    static gboolean onDraw(GtkWidget *widget, cairo_t *cr, gpointer data);

    static void onBotModeSelected(GtkWidget* w, gpointer p);
    static void onHotseatModeSelected(GtkWidget* w, gpointer p);
    static void onToggleCards(GtkWidget* w, gpointer p);
    static void onStartRound(GtkWidget* w, gpointer p);
    static void onCall(GtkWidget* w, gpointer p);
    static void onFold(GtkWidget* w, gpointer p);
    static void onRaise(GtkWidget* w, gpointer p);
};