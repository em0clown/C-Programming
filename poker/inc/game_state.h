#pragma once
#include "structures.h"
#include <vector>
#include <string>

class GameState {
private:
    void (*updateUI_callback)();

public:
    GameMode currentMode = MENU;
    std::vector<Card> deck;
    std::vector<Card> community;
    Player p1, p2;
    int pot = 0;
    int currentBet = 0;
    int roundStage = 0;
    int activePlayerNum = 1;
    int dealerNum = 2;
    int lastRaiserNum = 1;
    bool roundActive = false;
    bool cardsHidden = true;
    std::string statusMessage = "Выберите режим игры в меню сверху!";

    GameState(void (*uiUpdater)());

    void createDeck();
    void shuffleDeck();
    Card dealCard();
    void toggleActivePlayer();
    void executeBotTurn();
    void nextStage();
    void switchMode(GameMode mode);
    void startRound();

    bool isAnyPlayerBroke();
    bool canAffordAnte();
    void resetGame();
};