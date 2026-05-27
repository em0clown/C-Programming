#include <gtk/gtk.h>
#include <ctime>
#include <cstdlib>
#include <random>
#include "game_state.h"
#include "game_view.h"
#include <clocale>

int main(int argc, char *argv[]) {
    std::setlocale(LC_ALL, "");
    srand(time(nullptr));
    gtk_init(&argc, &argv);

    GameState gameState(GameView::updateUI);
    GameView::init(&gameState);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Texas poker");
    gtk_window_set_default_size(GTK_WINDOW(window), 980, 750);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget* menuBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_box_pack_start(GTK_BOX(vbox), menuBox, FALSE, FALSE, 4);

    GtkWidget *btnBot = gtk_button_new_with_label(" РЕЖИМ: С БОТОМ ");
    g_signal_connect(btnBot, "clicked", G_CALLBACK(GameView::onBotModeSelected), nullptr);
    gtk_box_pack_start(GTK_BOX(menuBox), btnBot, TRUE, TRUE, 2);

    GtkWidget *btnHot = gtk_button_new_with_label(" РЕЖИМ: МУЛЬТИПЛЕЕР (HOTSEAT)");
    g_signal_connect(btnHot, "clicked", G_CALLBACK(GameView::onHotseatModeSelected), nullptr);
    gtk_box_pack_start(GTK_BOX(menuBox), btnHot, TRUE, TRUE, 2);

    GameView::drawingArea = gtk_drawing_area_new();
    gtk_widget_set_size_request(GameView::drawingArea, 900, 480);
    gtk_box_pack_start(GTK_BOX(vbox), GameView::drawingArea, TRUE, TRUE, 0);
    g_signal_connect(GameView::drawingArea, "draw", G_CALLBACK(GameView::onDraw), nullptr);

    GameView::moneyLabel = gtk_label_new("");
    GameView::statusLabel = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(vbox), GameView::moneyLabel, FALSE, FALSE, 2);
    
    gtk_box_pack_start(GTK_BOX(vbox), GameView::statusLabel, FALSE, FALSE, 2);

    GtkWidget* buttonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_box_pack_start(GTK_BOX(vbox), buttonBox, FALSE, FALSE, 10);

    auto add_game_btn = [&](const std::string& label, GCallback cb) {
        GtkWidget *btn = gtk_button_new_with_label(label.c_str());
        g_signal_connect(btn, "clicked", cb, nullptr);
        gtk_box_pack_start(GTK_BOX(buttonBox), btn, TRUE, TRUE, 2);
    };

    add_game_btn("НАЧАТЬ РАУНД", G_CALLBACK(GameView::onStartRound));
    add_game_btn("ПОКАЗАТЬ / СКРЫТЬ КАРТЫ", G_CALLBACK(GameView::onToggleCards));
    add_game_btn("СБРОС (FOLD)", G_CALLBACK(GameView::onFold));
    add_game_btn("ЧЕК / УРАВНЯТЬ (CALL)", G_CALLBACK(GameView::onCall));
    add_game_btn("ПОДНЯТЬ (RAISE)", G_CALLBACK(GameView::onRaise));

    gtk_widget_show_all(window);
    GameView::updateUI();

    gtk_main();
    return 0;
}