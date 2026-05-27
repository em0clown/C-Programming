#include "game_view.h"
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <cstring>

using namespace std;

GameState* GameView::state = nullptr;
GtkWidget* GameView::drawingArea = nullptr;
GtkWidget* GameView::statusLabel = nullptr;
GtkWidget* GameView::moneyLabel = nullptr;
cairo_surface_t* GameView::background = nullptr;
int GameView::bgWidth = 0;
int GameView::bgHeight = 0;

void log_write(const string& msg) {
    ofstream file("poker.log", ios::app);
    time_t now = time(0);
    char* dt = ctime(&now);
    dt[strlen(dt)-1] = '\0';
    file << "[" << dt << "] " << msg << endl;
    file.close();
}

void GameView::init(GameState* gameState) {
    state = gameState;

    const char* images[] = {"table.png", NULL};

    for (int i = 0; images[i] != NULL; i++) {
        background = cairo_image_surface_create_from_png(images[i]);
        if (cairo_surface_status(background) == CAIRO_STATUS_SUCCESS) {
            bgWidth = cairo_image_surface_get_width(background);
            bgHeight = cairo_image_surface_get_height(background);
            log_write(string("Background loaded: ") + images[i] + " (" + to_string(bgWidth) + "x" + to_string(bgHeight) + ")");
            break;
        }
        cairo_surface_destroy(background);
        background = nullptr;
    }

    if (background == nullptr) {
        log_write("No background image found, using solid color");
    }
}

void GameView::updateUI() {
    if (!state) return;

    if (state->currentMode != MENU) {
        string p2Name = (state->currentMode == SINGLE_BOT) ? "Бот" : "Игрок 2";

        string moneyText = "Игрок 1: $" + to_string(state->p1.money) +
                           " (ставка $" + to_string(state->p1.roundBet) + ") | " +
                           p2Name + ": $" + to_string(state->p2.money) +
                           " (ставка $" + to_string(state->p2.roundBet) + ") | " +
                           "БАНК: $" + to_string(state->pot) + " | " +
                           "СТАВКА: $" + to_string(state->currentBet);

        gtk_label_set_text(GTK_LABEL(moneyLabel), moneyText.c_str());
        gtk_label_set_text(GTK_LABEL(statusLabel), state->statusMessage.c_str());
    } else {
        gtk_label_set_text(GTK_LABEL(moneyLabel), "Ожидание выбора режима...");
        gtk_label_set_text(GTK_LABEL(statusLabel), "Выберите режим игры на панели сверху");
    }

    gtk_widget_queue_draw(drawingArea);
}

void GameView::drawCard(cairo_t *cr, double x, double y, double w, double h, Card card, bool faceUp) {
    cairo_rectangle(cr, x, y, w, h);

    if (faceUp) {
        cairo_set_source_rgb(cr, 1, 1, 1);
        cairo_fill_preserve(cr);
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_set_line_width(cr, 2);
        cairo_stroke(cr);

        string rank;
        if (card.rank == 11) rank = "J";
        else if (card.rank == 12) rank = "Q";
        else if (card.rank == 13) rank = "K";
        else if (card.rank == 14) rank = "A";
        else rank = to_string(card.rank);

        string suit;
        if (card.suit == 0) suit = "♥";
        else if (card.suit == 1) suit = "♦";
        else if (card.suit == 2) suit = "♣";
        else suit = "♠";

        if (card.suit == 0 || card.suit == 1) {
            cairo_set_source_rgb(cr, 0.9, 0, 0);
        } else {
            cairo_set_source_rgb(cr, 0, 0, 0);
        }

        cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 22);
        cairo_move_to(cr, x + 8, y + 28);
        cairo_show_text(cr, rank.c_str());

        cairo_set_font_size(cr, 38);
        cairo_move_to(cr, x + w/2 - 16, y + h/2 + 14);
        cairo_show_text(cr, suit.c_str());
    } else {
        cairo_set_source_rgb(cr, 0.15, 0.25, 0.55);
        cairo_fill_preserve(cr);
        cairo_set_source_rgb(cr, 0.9, 0.9, 0.9);
        cairo_set_line_width(cr, 2);
        cairo_stroke(cr);
    }
}

gboolean GameView::onDraw(GtkWidget *widget, cairo_t *cr, gpointer data) {
    int w = gtk_widget_get_allocated_width(widget);
    int h = gtk_widget_get_allocated_height(widget);

    if (background != nullptr) {
        cairo_save(cr);
        cairo_rectangle(cr, 0, 0, w, h);
        cairo_clip(cr);
        cairo_scale(cr, (double)w / bgWidth, (double)h / bgHeight);
        cairo_set_source_surface(cr, background, 0, 0);
        cairo_paint(cr);
        cairo_restore(cr);
    } else {
        cairo_set_source_rgb(cr, 0.07, 0.35, 0.12);
        cairo_rectangle(cr, 0, 0, w, h);
        cairo_fill(cr);

        cairo_set_source_rgba(cr, 0.05, 0.25, 0.08, 0.5);
        for (int i = 0; i < w; i += 60) {
            for (int j = 0; j < h; j += 60) {
                cairo_rectangle(cr, i, j, 30, 30);
                cairo_fill(cr);
            }
        }
    }

    if (state->currentMode == MENU) {
        cairo_set_source_rgba(cr, 0, 0, 0, 0.7);
        cairo_rectangle(cr, 0, 0, w, h);
        cairo_fill(cr);
        cairo_set_source_rgb(cr, 1, 1, 1);
        cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 32);
        cairo_move_to(cr, w/2 - 220, h/2 - 30);
        cairo_show_text(cr, "ТЕХАССКИЙ ПОКЕР");
        cairo_set_font_size(cr, 18);
        cairo_move_to(cr, w/2 - 200, h/2 + 20);
        cairo_show_text(cr, "Выберите режим игры на панели сверху");
        return FALSE;
    }

    double cardW = 75;
    double cardH = 108;

    if (!state->community.empty()) {
        double startX = (w - state->community.size() * (cardW + 12)) / 2;
        for (size_t i = 0; i < state->community.size(); i++) {
            drawCard(cr, startX + i * (cardW + 12), h/2 - cardH/2, cardW, cardH, state->community[i], true);
        }
    }

    if (!state->p2.hand.empty()) {
        bool showP2 = (!state->roundActive && !state->p2.isFolded) ||
                      (state->roundActive && state->activePlayerNum == 2 && !state->cardsHidden);
        drawCard(cr, w/2 - cardW - 8, 40, cardW, cardH, state->p2.hand[0], showP2);
        drawCard(cr, w/2 + 8, 40, cardW, cardH, state->p2.hand[1], showP2);
    }

    if (!state->p1.hand.empty()) {
        bool showP1 = (!state->roundActive && !state->p1.isFolded) ||
                      (state->roundActive && state->activePlayerNum == 1 && !state->cardsHidden);
        drawCard(cr, w/2 - cardW - 8, h - 160, cardW, cardH, state->p1.hand[0], showP1);
        drawCard(cr, w/2 + 8, h - 160, cardW, cardH, state->p1.hand[1], showP1);
    }

    cairo_set_source_rgb(cr, 1, 1, 0.9);
    cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 18);

    if (state->roundActive && state->activePlayerNum == 2) {
        cairo_set_source_rgb(cr, 1, 0.85, 0);
    } else {
        cairo_set_source_rgb(cr, 1, 1, 0.9);
    }
    cairo_move_to(cr, w/2 - 55, 25);
    if (state->currentMode == SINGLE_BOT) {
        cairo_show_text(cr, "БОТ");
    } else {
        cairo_show_text(cr, "ИГРОК 2");
    }

    if (state->roundActive && state->activePlayerNum == 1) {
        cairo_set_source_rgb(cr, 1, 0.85, 0);
    } else {
        cairo_set_source_rgb(cr, 1, 1, 0.9);
    }
    cairo_move_to(cr, w/2 - 55, h - 175);
    cairo_show_text(cr, "ИГРОК 1");

    if (state->currentMode == MULTI_HOTSEAT && state->roundActive && state->cardsHidden) {
        cairo_set_source_rgba(cr, 0, 0, 0, 0.85);
        if (state->activePlayerNum == 1) {
            cairo_rectangle(cr, 0, h - 190, w, 190);
        } else {
            cairo_rectangle(cr, 0, 0, w, 190);
        }
        cairo_fill(cr);

        cairo_set_source_rgb(cr, 1, 1, 1);
        cairo_set_font_size(cr, 20);
        double messageY = (state->activePlayerNum == 1) ? h - 110 : 100;
        cairo_move_to(cr, w/2 - 250, messageY);
        cairo_show_text(cr, "ЭКРАН СКРЫТ ДЛЯ ХОДА. Нажмите кнопку просмотра карт");
    }

    return FALSE;
}

void GameView::onBotModeSelected(GtkWidget* w, gpointer p) {
    state->switchMode(SINGLE_BOT);
    log_write("Mode switched to: SINGLE_BOT");
}

void GameView::onHotseatModeSelected(GtkWidget* w, gpointer p) {
    state->switchMode(MULTI_HOTSEAT);
    log_write("Mode switched to: MULTI_HOTSEAT");
}

void GameView::onToggleCards(GtkWidget* w, gpointer p) {
    if (!state->roundActive || state->currentMode == SINGLE_BOT) return;
    state->cardsHidden = !state->cardsHidden;
    if (state->cardsHidden) {
        state->statusMessage = "Карты скрыты. Передайте устройство сопернику!";
    } else {
        state->statusMessage = "Ваш ход, Игрок " + to_string(state->activePlayerNum) + "!";
    }
    updateUI();
}

void GameView::onStartRound(GtkWidget* w, gpointer p) {
    state->startRound();
    log_write("Round started");
}

void GameView::onCall(GtkWidget* w, gpointer p) {
    if (!state->roundActive || state->cardsHidden) return;

    Player& current = (state->activePlayerNum == 1) ? state->p1 : state->p2;
    int callAmount = state->currentBet - current.roundBet;

    if (current.money == 0 && callAmount > 0) {
        state->statusMessage = "У вас 0 фишек! Нажмите 'НАЧАТЬ РАУНД' для восстановления.";
        updateUI();
        return;
    }

    if (current.money >= callAmount) {
        current.money -= callAmount;
        state->pot += callAmount;
        current.roundBet += callAmount;
        log_write("Player " + to_string(state->activePlayerNum) + " called " + to_string(callAmount));
        state->toggleActivePlayer();
    } else {
        state->statusMessage = "Недостаточно фишек!";
        updateUI();
    }
}

void GameView::onFold(GtkWidget* w, gpointer p) {
    if (!state->roundActive || state->cardsHidden) return;

    state->roundActive = false;
    state->cardsHidden = false;

    if (state->activePlayerNum == 1) {
        state->p1.isFolded = true;
        state->p2.money += state->pot;
        state->statusMessage = "Игрок 1 сдался (Fold). Банк уходит оппоненту!";
        log_write("Player 1 folded");
    } else {
        state->p2.isFolded = true;
        state->p1.money += state->pot;
        if (state->currentMode == SINGLE_BOT) {
            state->statusMessage = "Бот сдался (Fold). Игрок 1 забирает банк!";
        } else {
            state->statusMessage = "Игрок 2 сдался (Fold). Игрок 1 забирает банк!";
        }
        log_write("Player 2 folded");
    }

    state->pot = 0;
    updateUI();
}

void GameView::onRaise(GtkWidget* w, gpointer p) {
    if (!state->roundActive || state->cardsHidden) return;

    Player& current = (state->activePlayerNum == 1) ? state->p1 : state->p2;
    int minRaise = state->currentBet + 20;
    string msgText = "Введите ОБЩУЮ ставку (минимум " + to_string(minRaise) + "):";

    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL, "%s", msgText.c_str());
    GtkWidget *entry = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), entry);
    gtk_widget_show(entry);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        int targetBet = atoi(gtk_entry_get_text(GTK_ENTRY(entry)));
        int addedCost = targetBet - current.roundBet;

        if (targetBet >= minRaise && current.money >= addedCost) {
            current.money -= addedCost;
            state->pot += addedCost;
            current.roundBet = targetBet;
            state->currentBet = targetBet;
            state->lastRaiserNum = state->activePlayerNum;
            log_write("Player " + to_string(state->activePlayerNum) + " raised to " + to_string(targetBet));
            gtk_widget_destroy(dialog);
            state->toggleActivePlayer();
            return;
        } else {
            state->statusMessage = "Неверная сумма или недостаточно фишек!";
            updateUI();
        }
    }

    gtk_widget_destroy(dialog);
}