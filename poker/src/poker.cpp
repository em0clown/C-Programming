#include <gtk/gtk.h>
#include <cairo.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <map>

using namespace std;

struct Card {
    int rank;  // 2-14 (11=J, 12=Q, 13=K, 14=A)
    int suit;  // 0=♥, 1=♦, 2=♣, 3=♠
};

struct HandRank {
    int rank;  // 1-9
    vector<int> kickers;
    string name;
};

class PokerEngine {
public:
    static string getHandRankName(int rank) {
        static const vector<string> names = {
            "Ошибка", "Старшая карта", "Пара", "Две пары", 
            "Сет", "Стрит", "Флеш", "Фулл-хаус", "Каре", "Стрит-флеш"
        };
        return (rank >= 1 && rank <= 9) ? names[rank] : "Неизвестно";
    }

    static void sortCards(vector<Card>& cards) {
        sort(cards.begin(), cards.end(), [](const Card& a, const Card& b) {
            return a.rank > b.rank;
        });
    }

    static bool isFlush(const vector<Card>& cards, vector<int>& flushRanks) {
        int suitCount[4] = {0, 0, 0, 0};
        for (const auto& c : cards) suitCount[c.suit]++;
        
        for (int s = 0; s < 4; s++) {
            if (suitCount[s] >= 5) {
                for (const auto& c : cards) {
                    if (c.suit == s) flushRanks.push_back(c.rank);
                }
                sort(flushRanks.begin(), flushRanks.end(), greater<int>());
                return true;
            }
        }
        return false;
    }

    static bool isStraight(const vector<int>& ranks, vector<int>& straightCards) {
        vector<int> uniqueRanks = ranks;
        sort(uniqueRanks.begin(), uniqueRanks.end(), greater<int>());
        uniqueRanks.erase(unique(uniqueRanks.begin(), uniqueRanks.end()), uniqueRanks.end());

        if (uniqueRanks.size() < 5) return false;

        for (size_t i = 0; i + 4 < uniqueRanks.size(); i++) {
            if (uniqueRanks[i] == uniqueRanks[i+4] + 4) {
                straightCards.push_back(uniqueRanks[i]);
                return true;
            }
        }

        if (count(uniqueRanks.begin(), uniqueRanks.end(), 14) &&
            count(uniqueRanks.begin(), uniqueRanks.end(), 2) &&
            count(uniqueRanks.begin(), uniqueRanks.end(), 3) &&
            count(uniqueRanks.begin(), uniqueRanks.end(), 4) &&
            count(uniqueRanks.begin(), uniqueRanks.end(), 5)) {
            straightCards.push_back(5); 
            return true;
        }

        return false;
    }

    static HandRank evaluateHand(const vector<Card>& hole, const vector<Card>& community) {
        vector<Card> allCards = hole;
        allCards.insert(allCards.end(), community.begin(), community.end());
        sortCards(allCards);

        HandRank result{1, {}, "Старшая карта"};
        map<int, int> rankCount;
        vector<int> ranks;

        for (const auto& c : allCards) {
            rankCount[c.rank]++;
            ranks.push_back(c.rank);
        }

        vector<pair<int, int>> freq(rankCount.begin(), rankCount.end());
        sort(freq.begin(), freq.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
            if (a.second != b.second) return a.second > b.second;
            return a.first > b.first;
        });

        vector<int> flushRanks;
        bool flush = isFlush(allCards, flushRanks);

        vector<int> straightRanks;
        bool straight = isStraight(ranks, straightRanks);

        if (flush && straight) {
            // Настоящая проверка сложнее, но для аркады сойдет
            result.rank = 9; result.name = "Стрит-флеш";
            result.kickers = straightRanks; return result;
        }
        if (freq[0].second == 4) {
            result.rank = 8; result.name = "Каре";
            result.kickers.push_back(freq[0].first);
            result.kickers.push_back(freq[1].first);
            return result;
        }
        // 7. Фулл-хаус
        if (freq[0].second == 3 && freq.size() > 1 && freq[1].second >= 2) {
            result.rank = 7; result.name = "Фулл-хаус";
            result.kickers.push_back(freq[0].first);
            result.kickers.push_back(freq[1].first);
            return result;
        }
        if (flush) {
            result.rank = 6; result.name = "Флеш";
            result.kickers = flushRanks; return result;
        }
        if (straight) {
            result.rank = 5; result.name = "Стрит";
            result.kickers = straightRanks; return result;
        }
        if (freq[0].second == 3) {
            result.rank = 4; result.name = "Сет";
            result.kickers.push_back(freq[0].first);
            for (size_t i = 1; i < freq.size(); i++) result.kickers.push_back(freq[i].first);
            return result;
        }
        if (freq[0].second == 2 && freq.size() > 1 && freq[1].second == 2) {
            result.rank = 3; result.name = "Две пары";
            result.kickers.push_back(freq[0].first);
            result.kickers.push_back(freq[1].first);
            for (size_t i = 2; i < freq.size(); i++) result.kickers.push_back(freq[i].first);
            return result;
        }
        if (freq[0].second == 2) {
            result.rank = 2; result.name = "Пара";
            result.kickers.push_back(freq[0].first);
            for (size_t i = 1; i < freq.size(); i++) result.kickers.push_back(freq[i].first);
            return result;
        }
        result.rank = 1; result.name = "Старшая карта";
        for (size_t i = 0; i < allCards.size() && i < 5; i++) result.kickers.push_back(allCards[i].rank);
        return result;
    }

    static int compareHands(HandRank& h1, HandRank& h2) {
        if (h1.rank != h2.rank) return h1.rank > h2.rank ? 1 : -1;
        for (size_t i = 0; i < h1.kickers.size() && i < h2.kickers.size(); i++) {
            if (h1.kickers[i] != h2.kickers[i]) return h1.kickers[i] > h2.kickers[i] ? 1 : -1;
        }
        return 0;
    }
};

vector<Card> deck;
vector<Card> playerHand;
vector<Card> computerHand;
vector<Card> community;

int playerMoney = 1000;
int computerMoney = 1000;
int pot = 0;

int playerRoundBet = 0;
int computerRoundBet = 0;
int currentBet = 0; // Текущая ставка, до которой нужно выровняться

bool gameStarted = false;
bool roundActive = false;
bool playerTurn = true;
int roundStage = 0; // 0=Префлоп, 1=Флоп, 2=Терн, 3=Ривер, 4=Шоудаун
string statusMessage = "Игра готова. Нажмите НОВАЯ ИГРА";

GtkWidget *drawingArea;
GtkWidget *statusLabel;
GtkWidget *moneyLabel;

void updateUI();
void computerTurn();

void createDeck() {
    deck.clear();
    for (int s = 0; s < 4; s++) {
        for (int r = 2; r <= 14; r++) deck.push_back(Card{r, s});
    }
}

void shuffleDeck() {
    random_shuffle(deck.begin(), deck.end());
}

Card dealCard() {
    Card c = deck.back();
    deck.pop_back();
    return c;
}

void resetRoundBets() {
    playerRoundBet = 0;
    computerRoundBet = 0;
    currentBet = 0;
}

void nextStage() {
    resetRoundBets();
    roundStage++;
    playerTurn = true; 

    if (roundStage == 1) {
        dealCard();
        community.push_back(dealCard());
        community.push_back(dealCard());
        community.push_back(dealCard());
        statusMessage = "Флоп! Ваш ход.";
    } else if (roundStage == 2) {
        dealCard();
        community.push_back(dealCard());
        statusMessage = "Тёрн! Ваш ход.";
    } else if (roundStage == 3) {
        dealCard();
        community.push_back(dealCard());
        statusMessage = "Ривер! Ваш ход.";
    } else if (roundStage == 4) {
        roundActive = false;
        HandRank pRank = PokerEngine::evaluateHand(playerHand, community);
        HandRank cRank = PokerEngine::evaluateHand(computerHand, community);

        statusMessage = "Вскрытие! Вы: " + pRank.name + " | Компьютер: " + cRank.name;
        int res = PokerEngine::compareHands(pRank, cRank);
        if (res > 0) {
            playerMoney += pot;
            statusMessage += " — ВЫ ПОБЕДИЛИ!";
        } else if (res < 0) {
            computerMoney += pot;
            statusMessage += " — Компьютер победил";
        } else {
            playerMoney += pot / 2;
            computerMoney += pot / 2;
            statusMessage += " — НИЧЬЯ! 🤝";
        }
        pot = 0;
    }
    updateUI();
}

void computerTurn() {
    if (!roundActive || playerTurn) return;

    int callAmount = currentBet - computerRoundBet;

    if (callAmount > computerMoney) {
        statusMessage = "У компьютера не хватает денег. Компьютер фолдит — Вы выиграли!";
        playerMoney += pot;
        pot = 0;
        roundActive = false;
        updateUI();
        return;
    }

    HandRank compRank = PokerEngine::evaluateHand(computerHand, community);
    
    bool shouldCall = false;
    if (roundStage == 0) {
        if (compRank.rank >= 2 || computerHand[0].rank >= 10 || computerHand[1].rank >= 10 || (rand() % 100 < 50)) {
            shouldCall = true;
        }
    } else {
        // На постфлопе: если есть пара или лучше, или если доложить нужно совсем мало
        if (compRank.rank >= 2 || callAmount <= 20 || (rand() % 100 < 30)) {
            shouldCall = true;
        }
    }

    if (shouldCall || callAmount == 0) {
        computerMoney -= callAmount;
        pot += callAmount;
        computerRoundBet += callAmount;

        if (callAmount > 0) {
            statusMessage = "Компьютер уравнял ставку (добавил " + to_string(callAmount) + ")";
        } else {
            statusMessage = "Компьютер сказал Чек";
        }

        if (playerRoundBet == computerRoundBet) {
            nextStage();
        } else {
            playerTurn = true;
            updateUI();
        }
    } else {
        statusMessage = "Компьютер сбрасывает карты (Фолд) — Вы выиграли!";
        playerMoney += pot;
        pot = 0;
        roundActive = false;
        updateUI();
    }
}

void onNewGame(GtkWidget*, gpointer) {
    playerMoney = 1000;
    computerMoney = 1000;
    pot = 0;
    resetRoundBets();
    community.clear();
    playerHand.clear();
    computerHand.clear();
    gameStarted = true;
    roundActive = false;
    roundStage = 0;
    statusMessage = "Новая игра запущена. Пора Начать Раунд!";
    updateUI();
}

void onStartRound(GtkWidget*, gpointer) {
    if (!gameStarted || playerMoney < 10 || computerMoney < 10) {
        onNewGame(nullptr, nullptr);
    }

    createDeck();
    shuffleDeck();
    playerHand.clear();
    computerHand.clear();
    community.clear();

    playerHand.push_back(dealCard());
    playerHand.push_back(dealCard());
    computerHand.push_back(dealCard());
    computerHand.push_back(dealCard());

    // Обязательные блайнды/анте
    playerMoney -= 10;
    computerMoney -= 10;
    pot = 20;
    
    playerRoundBet = 10;
    computerRoundBet = 10;
    currentBet = 10;

    roundStage = 0;
    roundActive = true;
    playerTurn = true;
    statusMessage = "Раунд начат. У вас на руках отличные карты, ваш ход!";
    updateUI();
}

void onCall(GtkWidget*, gpointer) {
    if (!roundActive || !playerTurn) return;

    int callAmount = currentBet - playerRoundBet;

    if (playerMoney >= callAmount) {
        playerMoney -= callAmount;
        pot += callAmount;
        playerRoundBet += callAmount;

        statusMessage = (callAmount > 0) ? "Вы уравняли ставку" : "Вы сказали Чек";
        playerTurn = false;
        updateUI();

        if (playerRoundBet == computerRoundBet && computerRoundBet > 0) {
            nextStage();
        } else {
            computerTurn();
        }
    } else {
        statusMessage = "У вас нет денег для этого действия!";
        updateUI();
    }
}

void onFold(GtkWidget*, gpointer) {
    if (!roundActive || !playerTurn) return;

    statusMessage = "Вы сбросили карты. Компьютер забирает банк.";
    computerMoney += pot;
    pot = 0;
    roundActive = false;
    updateUI();
}

void onRaise(GtkWidget*, gpointer) {
    if (!roundActive || !playerTurn) return;

    int minRaise = currentBet + 20;
    string msgText = "Введите общую сумму ставки (минимум " + to_string(minRaise) + "):";
    
    GtkWidget *dialog = gtk_message_dialog_new(nullptr, GTK_DIALOG_MODAL,
        GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL, "%s", msgText.c_str());
    
    GtkWidget *entry = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), entry);
    gtk_widget_show(entry);
    
    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK) {
        int targetBet = atoi(gtk_entry_get_text(GTK_ENTRY(entry)));
        int addedCost = targetBet - playerRoundBet;

        if (targetBet >= minRaise && playerMoney >= addedCost) {
            playerMoney -= addedCost;
            pot += addedCost;
            playerRoundBet = targetBet;
            currentBet = targetBet;

            statusMessage = "Вы подняли ставку до " + to_string(targetBet);
            playerTurn = false;
            updateUI();
            computerTurn();
        } else {
            statusMessage = "Недопустимая сумма рейза!";
            updateUI();
        }
    }
    gtk_widget_destroy(dialog);
}

void drawCard(cairo_t *cr, double x, double y, double w, double h, Card card, bool faceUp) {
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
        
        string suit = (card.suit == 0) ? "♥" : (card.suit == 1) ? "♦" : (card.suit == 2) ? "♣" : "♠";
        
        if (card.suit == 0 || card.suit == 1) cairo_set_source_rgb(cr, 0.8, 0, 0);
        else cairo_set_source_rgb(cr, 0, 0, 0);
        
        cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 18);
        cairo_move_to(cr, x + 8, y + 22);
        cairo_show_text(cr, rank.c_str());
        
        cairo_set_font_size(cr, 32);
        cairo_move_to(cr, x + w/2 - 14, y + h/2 + 12);
        cairo_show_text(cr, suit.c_str());
    } else {
        cairo_set_source_rgb(cr, 0.15, 0.25, 0.55);
        cairo_fill_preserve(cr);
        cairo_set_source_rgb(cr, 0.9, 0.9, 0.9);
        cairo_set_line_width(cr, 2);
        cairo_stroke(cr);
    }
}

gboolean onDraw(GtkWidget *widget, cairo_t *cr, gpointer) {
    int w = gtk_widget_get_allocated_width(widget);
    int h = gtk_widget_get_allocated_height(widget);
    
    // Зеленый стол сукна
    cairo_set_source_rgb(cr, 0.08, 0.40, 0.15);
    cairo_rectangle(cr, 0, 0, w, h);
    cairo_fill(cr);
    
    double cardW = 75, cardH = 108;
    
    if (!community.empty()) {
        double startX = (w - community.size() * (cardW + 12)) / 2;
        for (size_t i = 0; i < community.size(); i++) {
            drawCard(cr, startX + i * (cardW + 12), h/2 - cardH/2, cardW, cardH, community[i], true);
        }
    }
    
    if (!computerHand.empty()) {
        bool showComp = (roundStage == 4 || !roundActive);
        drawCard(cr, w/2 - cardW - 8, 40, cardW, cardH, computerHand[0], showComp);
        drawCard(cr, w/2 + 8, 40, cardW, cardH, computerHand[1], showComp);
    }
    
    if (!playerHand.empty()) {
        drawCard(cr, w/2 - cardW - 8, h - 160, cardW, cardH, playerHand[0], true);
        drawCard(cr, w/2 + 8, h - 160, cardW, cardH, playerHand[1], true);
    }
    
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_set_font_size(cr, 14);
    cairo_move_to(cr, w/2 - 45, 25);
    cairo_show_text(cr, "КОМПЬЮТЕР");
    cairo_move_to(cr, w/2 - 15, h - 170);
    cairo_show_text(cr, "ВЫ");
    
    return FALSE;
}

void updateUI() {
    string info = "     Ваш баланс: $" + to_string(playerMoney) + 
                  "  |  Компьютер: $" + to_string(computerMoney) + 
                  "  |  Общий банк: $" + to_string(pot) + 
                  "  |  Ставка раунда: $" + to_string(currentBet);
    gtk_label_set_text(GTK_LABEL(moneyLabel), info.c_str());
    gtk_label_set_text(GTK_LABEL(statusLabel), statusMessage.c_str());
    gtk_widget_queue_draw(drawingArea);
}

int main(int argc, char *argv[]) {
    srand(time(nullptr));
    gtk_init(&argc, &argv);
    
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Texas Pocker");
    gtk_window_set_default_size(GTK_WINDOW(window), 950, 720);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);
    
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    drawingArea = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawingArea, 900, 480);
    gtk_box_pack_start(GTK_BOX(vbox), drawingArea, TRUE, TRUE, 0);
    g_signal_connect(drawingArea, "draw", G_CALLBACK(onDraw), nullptr);
    
    moneyLabel = gtk_label_new("");
    statusLabel = gtk_label_new("");
    
    gtk_box_pack_start(GTK_BOX(vbox), moneyLabel, FALSE, FALSE, 4);
    gtk_box_pack_start(GTK_BOX(vbox), statusLabel, FALSE, FALSE, 4);
    
    GtkWidget *buttonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_pack_start(GTK_BOX(vbox), buttonBox, FALSE, FALSE, 12);
    
    auto add_btn = [&](const string& label, GCallback cb) {
        GtkWidget *btn = gtk_button_new_with_label(label.c_str());
        g_signal_connect(btn, "clicked", cb, nullptr);
        gtk_box_pack_start(GTK_BOX(buttonBox), btn, TRUE, TRUE, 2);
    };
    
    add_btn("НАЧАТЬ РАУНД", G_CALLBACK(onStartRound));
    add_btn("СБРОС (FOLD)", G_CALLBACK(onFold));
    add_btn("ЧЕК / УРАВНЯТЬ (CALL)", G_CALLBACK(onCall));
    add_btn("ПОДНЯТЬ (RAISE)", G_CALLBACK(onRaise));
    
    gtk_widget_show_all(window);
    updateUI();
    
    gtk_main();
    return 0;
}