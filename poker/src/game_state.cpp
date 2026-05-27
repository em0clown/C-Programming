#include "game_state.h"
#include "poker_engine.h"
#include <algorithm>
#include <cstdlib>
#include <random>

using namespace std;

GameState::GameState(void (*uiUpdater)()) : updateUI_callback(uiUpdater) {
    p1.money = 1000;
    p2.money = 1000;
}

void GameState::createDeck() {
    deck.clear();
    for (int s = 0; s < 4; s++) {
        for (int r = 2; r <= 14; r++) {
            deck.push_back(Card{r, s});
        }
    }
}

void GameState::shuffleDeck() {
    random_device rd;
    mt19937 g(rd());
    shuffle(deck.begin(), deck.end(), g);
}

Card GameState::dealCard() {
    Card c = deck.back();
    deck.pop_back();
    return c;
}

bool GameState::isAnyPlayerBroke() {
    return (p1.money < 10) || (p2.money < 10);
}

bool GameState::canAffordAnte() {
    return (p1.money >= 10 && p2.money >= 10);
}

void GameState::resetGame() {
    p1.money = 1000;
    p2.money = 1000;
    p1.roundBet = 0;
    p2.roundBet = 0;
    pot = 0;
    currentBet = 0;
    roundActive = false;
    p1.hand.clear();
    p2.hand.clear();
    community.clear();
    p1.isFolded = false;
    p2.isFolded = false;
    roundStage = 0;
    statusMessage = "Баланс всех игроков восстановлен до 1000 фишек. Нажмите НАЧАТЬ РАУНД.";
    updateUI_callback();
}

void GameState::toggleActivePlayer() {
    if (!roundActive) return;

    int nextPlayer = (activePlayerNum == 1) ? 2 : 1;

    if (nextPlayer == lastRaiserNum && ((nextPlayer == 1 ? p1.roundBet : p2.roundBet) == currentBet)) {
        nextStage();
    } else {
        activePlayerNum = nextPlayer;

        if (currentMode == MULTI_HOTSEAT) {
            cardsHidden = true;
            statusMessage = "Ход переходит к игроку " + to_string(activePlayerNum) + ". Подтвердите готовность!";
        } else if (currentMode == SINGLE_BOT && activePlayerNum == 2) {
            if (p2.money <= 0) {
                roundActive = false;
                p2.isFolded = true;
                p1.money += pot;
                statusMessage = "У бота закончились фишки! Игрок 1 выиграл банк! Нажмите НАЧАТЬ РАУНД.";
                pot = 0;
                updateUI_callback();
                return;
            }
            cardsHidden = false;
            statusMessage = "Думает бот...";
            updateUI_callback();
            executeBotTurn();
            return;
        }
        updateUI_callback();
    }
}

void GameState::executeBotTurn() {
    if (!roundActive) return;

    if (p2.money <= 0) {
        roundActive = false;
        p2.isFolded = true;
        p1.money += pot;
        statusMessage = "У бота закончились фишки! Игрок 1 выиграл банк! Нажмите НАЧАТЬ РАУНД.";
        pot = 0;
        updateUI_callback();
        return;
    }

    HandRank botHand = PokerEngine::evaluateHand(p2.hand, community);
    int callAmount = currentBet - p2.roundBet;

    if (callAmount > p2.money) {
        roundActive = false;
        p2.isFolded = true;
        p1.money += pot;
        statusMessage = "У бота недостаточно фишек для колла! Бот пасует. Вы забираете банк!";
        pot = 0;
        updateUI_callback();
        return;
    }

    if (roundStage == 0) {
        int maxRank = max(p2.hand[0].rank, p2.hand[1].rank);
        if (maxRank < 7 && p2.hand[0].rank != p2.hand[1].rank && callAmount > 40) {
            roundActive = false;
            p2.isFolded = true;
            p1.money += pot;
            statusMessage = "Бот решил сбросить карты (Fold). Вы выиграли банк!";
            pot = 0;
        } else if (maxRank >= 12 || p2.hand[0].rank == p2.hand[1].rank) {
            int raiseValue = currentBet + 30;
            int added = raiseValue - p2.roundBet;
            if (p2.money >= added) {
                p2.money -= added;
                pot += added;
                p2.roundBet = raiseValue;
                currentBet = raiseValue;
                lastRaiserNum = 2;
                statusMessage = "Бот делает РЕЙЗ до $" + to_string(currentBet);
            } else {
                if (p2.money >= callAmount) {
                    p2.money -= callAmount;
                    pot += callAmount;
                    p2.roundBet += callAmount;
                    statusMessage = "Бот сыграл КОЛЛ/ЧЕК.";
                }
            }
        } else {
            if (p2.money >= callAmount) {
                p2.money -= callAmount;
                pot += callAmount;
                p2.roundBet += callAmount;
                statusMessage = "Бот сыграл КОЛЛ/ЧЕК.";
            }
        }
    } else {
        if (botHand.rank >= 3) {
            int raiseValue = currentBet + 40;
            int added = raiseValue - p2.roundBet;
            if (p2.money >= added) {
                p2.money -= added;
                pot += added;
                p2.roundBet = raiseValue;
                currentBet = raiseValue;
                lastRaiserNum = 2;
                statusMessage = "Бот агрессивно поднимает (РЕЙЗ) до $" + to_string(currentBet);
            } else {
                if (p2.money >= callAmount) {
                    p2.money -= callAmount;
                    pot += callAmount;
                    p2.roundBet += callAmount;
                    statusMessage = "Бот поддерживает ставку (Колл/Чек).";
                }
            }
        } else if (botHand.rank == 2 || callAmount == 0) {
            if (p2.money >= callAmount) {
                p2.money -= callAmount;
                pot += callAmount;
                p2.roundBet += callAmount;
                statusMessage = "Бот поддерживает ставку (Колл/Чек).";
            }
        } else {
            roundActive = false;
            p2.isFolded = true;
            p1.money += pot;
            statusMessage = "Бот пасует (Fold). Вы забираете банк!";
            pot = 0;
        }
    }

    if (roundActive) {
        int nextPlayer = 1;
        if (nextPlayer == lastRaiserNum && (p1.roundBet == currentBet)) {
            nextStage();
        } else {
            activePlayerNum = 1;
            updateUI_callback();
        }
    } else {
        updateUI_callback();
    }
}

void GameState::nextStage() {
    p1.roundBet = 0;
    p2.roundBet = 0;
    currentBet = 0;
    roundStage++;

    if (roundStage == 1) {
        dealCard();
        community.push_back(dealCard());
        community.push_back(dealCard());
        community.push_back(dealCard());
        statusMessage = "Флоп открыт!";
    } else if (roundStage == 2) {
        dealCard();
        community.push_back(dealCard());
        statusMessage = "Тёрн открыт!";
    } else if (roundStage == 3) {
        dealCard();
        community.push_back(dealCard());
        statusMessage = "Ривер открыт! Финальный раунд ставок.";
    } else if (roundStage == 4) {
        roundActive = false;
        cardsHidden = false;

        HandRank r1 = PokerEngine::evaluateHand(p1.hand, community);
        HandRank r2 = PokerEngine::evaluateHand(p2.hand, community);

        string p2Name = (currentMode == SINGLE_BOT) ? "Бот" : "Игрок 2";
        statusMessage = "Вскрытие! Игрок 1: " + r1.name + " | " + p2Name + ": " + r2.name;
        int res = PokerEngine::compareHands(r1, r2);

        if (res > 0) {
            p1.money += pot;
            statusMessage += " - ИГРОК 1 ПОБЕДИЛ!";
        } else if (res < 0) {
            p2.money += pot;
            statusMessage += " - " + p2Name + " ПОБЕДИЛ!";
        } else {
            p1.money += pot / 2;
            p2.money += pot / 2;
            statusMessage += " - НИЧЬЯ!";
        }
        pot = 0;
        updateUI_callback();
        return;
    }

    activePlayerNum = 1;
    lastRaiserNum = 1;
    cardsHidden = (currentMode == MULTI_HOTSEAT);
    updateUI_callback();
}

void GameState::switchMode(GameMode mode) {
    currentMode = mode;
    roundActive = false;
    community.clear();
    p1.hand.clear();
    p2.hand.clear();
    pot = 0;
    currentBet = 0;

    if (mode == SINGLE_BOT) {
        statusMessage = "Режим: С ботом. Нажмите НАЧАТЬ РАУНД.";
    } else if (mode == MULTI_HOTSEAT) {
        statusMessage = "Режим: Hotseat (Вдвоем). Нажмите НАЧАТЬ РАУНД.";
    }
    updateUI_callback();
}

void GameState::startRound() {
    if (currentMode == MENU) return;

    if (!canAffordAnte()) {
        if (p1.money < 10 && p2.money < 10) {
            resetGame();
        } else if (p1.money < 10) {
            p1.money = 1000;
            statusMessage = "Игрок 1 обанкротился! Баланс восстановлен до 1000 фишек. Нажмите НАЧАТЬ РАУНД.";
        } else if (p2.money < 10) {
            p2.money = 1000;
            string p2Name = (currentMode == SINGLE_BOT) ? "Бот" : "Игрок 2";
            statusMessage = p2Name + " обанкротился! Баланс восстановлен до 1000 фишек. Нажмите НАЧАТЬ РАУНД.";
        }
        updateUI_callback();
        return;
    }

    createDeck();
    shuffleDeck();
    community.clear();
    p1.hand.clear();
    p2.hand.clear();
    p1.isFolded = false;
    p2.isFolded = false;

    p1.hand.push_back(dealCard());
    p1.hand.push_back(dealCard());
    p2.hand.push_back(dealCard());
    p2.hand.push_back(dealCard());

    p1.money -= 10;
    p2.money -= 10;
    p1.roundBet = 10;
    p2.roundBet = 10;
    pot = 20;
    currentBet = 10;

    roundStage = 0;
    roundActive = true;
    cardsHidden = (currentMode == MULTI_HOTSEAT);

    dealerNum = (dealerNum == 1) ? 2 : 1;
    activePlayerNum = dealerNum;
    lastRaiserNum = activePlayerNum;

    statusMessage = "Раунд начат! Игрок " + to_string(activePlayerNum) + ", делайте ход.";
    if (currentMode == SINGLE_BOT && activePlayerNum == 2) {
        executeBotTurn();
    }
    updateUI_callback();
}