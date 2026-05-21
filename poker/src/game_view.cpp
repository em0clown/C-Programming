#include "game_view.h"
#include <cstdlib>

using namespace std;

// Инициализация статических членов
GameState* GameView::state = nullptr;
GtkWidget* GameView::drawingArea = nullptr;
GtkWidget* GameView::statusLabel = nullptr;
GtkWidget* GameView::moneyLabel = nullptr;

void GameView::init(GameState* gameState) {
    state = gameState;
}

void GameView::updateUI() {
    if (!state) return;
    if (state->currentMode != MENU) {
        string p2Name = (state->currentMode == SINGLE_BOT) ? "Бот" : "Игрок 2";
        string info = "Игрок 1: $" + to_string(state->p1.money) + " (Ставка: $" + to_string(state->p1.roundBet) + ")" +
                      "   |   " + p2Name + ": $" + to_string(state->p2.money) + " (Ставка: $" + to_string(state->p2.roundBet) + ")" +
                      "   |   БАНК: $" + to_string(state->pot) + "   |   Ставка стола: $" + to_string(state->currentBet);
        gtk_label_set_text(GTK_LABEL(moneyLabel), info.c_str());
    } else {
        gtk_label_set_text(GTK_LABEL(moneyLabel), "Ожидание выбора режима...");
    }
    gtk_label_set_text(GTK_LABEL(statusLabel), state->statusMessage.c_str());
    gtk_widget_queue_draw(drawingArea);
}

void GameView::drawCard(cairo_t *cr, double x, double y, double w, double h, Card card, bool faceUp) {
    cairo_rectangle(cr, x, y, w, h);
    if (faceUp) {
        cairo_set_source_rgb(cr, 1, 1, 1); cairo_fill_preserve(cr);
        cairo_set_source_rgb(cr, 0, 0, 0); cairo_set_line_width(cr, 2); cairo_stroke(cr);

        string rank = (card.rank == 11) ? "J" : (card.rank == 12) ? "Q" : (card.rank == 13) ? "K" : (card.rank == 14) ? "A" : to_string(card.rank);
        string suit = (card.suit == 0) ? "♥" : (card.suit == 1) ? "♦" : (card.suit == 2) ? "♣" : "♠";

        if (card.suit == 0 || card.suit == 1) cairo_set_source_rgb(cr, 0.8, 0, 0);
        else cairo_set_source_rgb(cr, 0, 0, 0);

        cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 18); cairo_move_to(cr, x + 8, y + 22); cairo_show_text(cr, rank.c_str());
        cairo_set_font_size(cr, 32); cairo_move_to(cr, x + w/2 - 14, y + h/2 + 12); cairo_show_text(cr, suit.c_str());
    } else {
        cairo_set_source_rgb(cr, 0.15, 0.25, 0.55); cairo_fill_preserve(cr);
        cairo_set_source_rgb(cr, 0.9, 0.9, 0.9); cairo_set_line_width(cr, 2); cairo_stroke(cr);
    }
}

gboolean GameView::onDraw(GtkWidget *widget, cairo_t *cr, gpointer) {
    int w = gtk_widget_get_allocated_width(widget);
    int h = gtk_widget_get_allocated_height(widget);

    if (state->currentMode == MENU) {
        cairo_set_source_rgb(cr, 0.1, 0.15, 0.2); cairo_rectangle(cr, 0, 0, w, h); cairo_fill(cr);
        cairo_set_source_rgb(cr, 1, 1, 1); cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 26); cairo_move_to(cr, w/2 - 190, h/2 - 20);
        cairo_show_text(cr, "♠ ПОКЕР ХЕДЗ-АП ДЛЯ ДВОИХ ♥");
        cairo_set_font_size(cr, 14); cairo_move_to(cr, w/2 - 150, h/2 + 20);
        cairo_show_text(cr, "Выберите режим игры на панели меню сверху.");
        return FALSE;
    }

    cairo_set_source_rgb(cr, 0.08, 0.40, 0.15); cairo_rectangle(cr, 0, 0, w, h); cairo_fill(cr);
    double cardW = 75, cardH = 108;

    if (!state->community.empty()) {
        double startX = (w - state->community.size() * (cardW + 12)) / 2;
        for (size_t i = 0; i < state->community.size(); i++) {
            drawCard(cr, startX + i * (cardW + 12), h/2 - cardH/2, cardW, cardH, state->community[i], true);
        }
    }

    if (!state->p2.hand.empty()) {
        bool showP2 = (!state->roundActive && !state->p2.isFolded) || (state->roundActive && state->activePlayerNum == 2 && !state->cardsHidden);
        drawCard(cr, w/2 - cardW - 8, 40, cardW, cardH, state->p2.hand[0], showP2);
        drawCard(cr, w/2 + 8, 40, cardW, cardH, state->p2.hand[1], showP2);
    }

    if (!state->p1.hand.empty()) {
        bool showP1 = (!state->roundActive && !state->p1.isFolded) || (state->roundActive && state->activePlayerNum == 1 && !state->cardsHidden);
        drawCard(cr, w/2 - cardW - 8, h - 160, cardW, cardH, state->p1.hand[0], showP1);
        drawCard(cr, w/2 + 8, h - 160, cardW, cardH, state->p1.hand[1], showP1);
    }

    cairo_set_source_rgb(cr, 1, 1, 1); cairo_set_font_size(cr, 14);
    if (state->roundActive && state->activePlayerNum == 2) cairo_set_source_rgb(cr, 1, 0.85, 0);
    cairo_move_to(cr, w/2 - 35, 25);
    cairo_show_text(cr, (state->currentMode == SINGLE_BOT) ? "БОТ (КОМПЬЮТЕР)" : "ИГРОК 2");

    cairo_set_source_rgb(cr, 1, 1, 1);
    if (state->roundActive && state->activePlayerNum == 1) cairo_set_source_rgb(cr, 1, 0.85, 0);
    cairo_move_to(cr, w/2 - 35, h - 175);
    cairo_show_text(cr, "ИГРОК 1");

    if (state->currentMode == MULTI_HOTSEAT && state->roundActive && state->cardsHidden) {
        cairo_set_source_rgba(cr, 0.05, 0.05, 0.1, 0.94);
        if (state->activePlayerNum == 1) cairo_rectangle(cr, 0, h - 190, w, 190);
        else cairo_rectangle(cr, 0, 0, w, 190);
        cairo_fill(cr);

        cairo_set_source_rgb(cr, 1, 1, 1); cairo_set_font_size(cr, 15);
        double messageY = (state->activePlayerNum == 1) ? h - 100 : 95;
        cairo_move_to(cr, w/2 - 220, messageY);
        cairo_show_text(cr, "ЭКРАН СКРЫТ ДЛЯ ХОДА. Нажмите кнопку просмотра карт.");
    }
    return FALSE;
}

void GameView::onBotModeSelected(GtkWidget*, gpointer) { state->switchMode(SINGLE_BOT); }
void GameView::onHotseatModeSelected(GtkWidget*, gpointer) { state->switchMode(MULTI_HOTSEAT); }

void GameView::onToggleCards(GtkWidget*, gpointer) {
    if (!state->roundActive || state->currentMode == SINGLE_BOT) return;
    state->cardsHidden = !state->cardsHidden;
    state->statusMessage = state->cardsHidden ? "Карты скрыты." : "Ваш ход, Игрок " + to_string(state->activePlayerNum) + "!";
    updateUI();
}

void GameView::onStartRound(GtkWidget*, gpointer) { state->startRound(); }

void GameView::onCall(GtkWidget*, gpointer) {
    if (!state->roundActive || state->cardsHidden) return;
    Player& current = (state->activePlayerNum == 1) ? state->p1 : state->p2;
    int callAmount = state->currentBet - current.roundBet;

    if (current.money >= callAmount) {
        current.money -= callAmount; state->pot += callAmount; current.roundBet += callAmount;
        state->toggleActivePlayer();
    } else {
        state->statusMessage = "Недостаточно фишек!"; updateUI();
    }
}

void GameView::onFold(GtkWidget*, gpointer) {
    if (!state->roundActive || state->cardsHidden) return;
    state->roundActive = false; state->cardsHidden = false;
    
    if (state->activePlayerNum == 1) {
        state->p1.isFolded = true; state->p2.money += state->pot;
        state->statusMessage = "Игрок 1 сдался (Fold). Банк уходит оппоненту!";
    } else {
        state->p2.isFolded = true; state->p1.money += state->pot;
        state->statusMessage = ((state->currentMode == SINGLE_BOT) ? "Бот" : "Игрок 2") + string(" сдался (Fold). Игрок 1 забирает банк!");
    }
    state->pot = 0; updateUI();
}

void GameView::onRaise(GtkWidget*, gpointer) {
    if (!state->roundActive || state->cardsHidden) return;
    Player& current = (state->activePlayerNum == 1) ? state->p1 : state->p2;
    int minRaise = state->currentBet + 20;
    string msgText = "Введите ОБЩУЮ ставку (минимум " + to_string(minRaise) + "):";

    GtkWidget *dialog = gtk_message_dialog_new(nullptr, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL, "%s", msgText.c_str());
    GtkWidget *entry = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), entry);
    gtk_widget_show(entry);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        int targetBet = atoi(gtk_entry_get_text(GTK_ENTRY(entry)));
        int addedCost = targetBet - current.roundBet;

        if (targetBet >= minRaise && current.money >= addedCost) {
            current.money -= addedCost; state->pot += addedCost;
            current.roundBet = targetBet; state->currentBet = targetBet;
            state->lastRaiserNum = state->activePlayerNum;
            gtk_widget_destroy(dialog);
            state->toggleActivePlayer();
            return;
        }
    }
    gtk_widget_destroy(dialog);
}