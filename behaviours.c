#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "functions.h"

int decideWhetherToBuyProperty(GameState *game, int playerIndex, int boardIndex) {
    PropertySquare *property = findProperty(game, boardIndex);
    PlayerName strategy = game->players[playerIndex].name;

    switch (strategy) {
        case AGGRESSIVE_INVESTOR:
            return (game->players[playerIndex].cash - property->purchasePrice) >= property->baseRent; //Use own rent as an indicator
        case CONSERVATIVE_BANKER:
            return (game->players[playerIndex].cash - property->purchasePrice) >= game->players[playerIndex].cash / 2;
        case RISK_TAKER:
            return 1;
        case OPPORTUNISTIC_TRADER: {
            int appreciationGain = 0;
            if (property->group == game->economy.marketBoomGroup && game->currentRound < game->economy.marketBoomExpiryRound) {
                appreciationGain = property->purchasePrice * 20 / 100;
            }
            return appreciationGain > property->houseCost;
        }
        default:
            return 0;
    }
}

int decideWhetherToBuyRailway(GameState *game, int playerIndex, int boardIndex) {
    RailwaySquare *railway = findRailway(game, boardIndex);
    PlayerName strategy = game->players[playerIndex].name;

    switch (strategy) {
        case AGGRESSIVE_INVESTOR: {
            int stationsOwned = 1; // counting this one being purchased
            for (int i = 0; i < 4; i++) {
                if (game->railways[i].owner == game->players[playerIndex].name) {
                    stationsOwned++;
                }
            }
            int maxOpponentStations = 4 - stationsOwned;
            int worstCaseRent;
            switch (maxOpponentStations) {
                case 0: worstCaseRent = 0; break;
                case 1: worstCaseRent = 250; break;
                case 2: worstCaseRent = 500; break;
                case 3: worstCaseRent = 1000; break;
                case 4: worstCaseRent = 2000; break;
                default: worstCaseRent = 0; break;
            }
            return (game->players[playerIndex].cash - railway->purchasePrice) >= worstCaseRent;
        }
        
        case CONSERVATIVE_BANKER:
            return game->players[playerIndex].cash >= railway->purchasePrice;
        case RISK_TAKER:
            return game->players[playerIndex].cash >= railway->purchasePrice;
        case OPPORTUNISTIC_TRADER:
            return game->players[playerIndex].cash >= railway->purchasePrice; 
        default:
            return 0;
    }
}

int decideWhetherToBuyUtility(GameState *game, int playerIndex, int boardIndex) {
    UtilitySquare *utility = findUtility(game, boardIndex);
    PlayerName strategy = game->players[playerIndex].name;

    switch (strategy) {
        case AGGRESSIVE_INVESTOR: {
            int utilitiesOwned = 1;
            for (int i = 0; i < 2; i++) {
                if (game->utilities[i].owner == game->players[playerIndex].name) {
                    utilitiesOwned++;
                }
            }
            int worstCaseRent;
            if (utilitiesOwned == 2) {
                worstCaseRent = 0;
            } else {
                worstCaseRent = 12 * 4;
            }
            return (game->players[playerIndex].cash - utility->purchasePrice) >= worstCaseRent;
        }
        case CONSERVATIVE_BANKER:
            return game->players[playerIndex].cash >= utility->purchasePrice;
        case RISK_TAKER:
            return game->players[playerIndex].cash >= utility->purchasePrice;
        case OPPORTUNISTIC_TRADER:
            return game->players[playerIndex].cash >= utility->purchasePrice;
        default:
            return 0;
    }
}

int decideAuctionBid(GameState *game, int playerIndex, int basePrice, int currentBid) {
    PlayerName strategy = game->players[playerIndex].name;

    switch (strategy) {
        case AGGRESSIVE_INVESTOR:
            return (currentBid + 250) <= (basePrice * 120 / 100) && game->players[playerIndex].cash >= (currentBid + 250);

        case CONSERVATIVE_BANKER:
            return (currentBid + 250) < basePrice && game->players[playerIndex].cash >= (currentBid + 250);

        case RISK_TAKER:
            return game->players[playerIndex].cash >= (currentBid + 250);

        case OPPORTUNISTIC_TRADER:
            return (currentBid + 250) < basePrice && game->players[playerIndex].cash >= (currentBid + 250);

        default:
            return 0;
    }
}

int decideWhetherToBuild(GameState *game, int playerIndex, int boardIndex) {
    PropertySquare *property = findProperty(game, boardIndex);
    PlayerName strategy = game->players[playerIndex].name;

    switch (strategy) {
        case AGGRESSIVE_INVESTOR:
            return 1;
        case CONSERVATIVE_BANKER:
            if (property->numBuildings == 4 && game->players[playerIndex].loan.amount > 0) {
                return 0;
            }
            return 1;
        case RISK_TAKER:
            return 1;
        case OPPORTUNISTIC_TRADER: {
            if (game->currentRound == game->economy.lastInflationRound) {
                return 0;
            }
            if (hasActiveCard(&game->players[playerIndex], 5, game->currentRound)) {
                return 1;
            }
            return 1;
        }
        default:
            return 0;
    }
}

int decideInsurancePolicy(GameState *game, int playerIndex, int boardIndex) {
    PropertySquare *property = findProperty(game, boardIndex);
    PlayerName strategy = game->players[playerIndex].name;

    switch (strategy) {
        case AGGRESSIVE_INVESTOR:
            if (property->numBuildings == 5) {
                return COMPREHENSIVE;
            }
            return BASIC;
        case CONSERVATIVE_BANKER:
            return COMPREHENSIVE;
        case RISK_TAKER:
            if (game->players[playerIndex].hasExperiencedLoss) {
                return COMPREHENSIVE;
            }
            return NONE;
        case OPPORTUNISTIC_TRADER:
            if (property->numBuildings == 5) {
                return COMPREHENSIVE;
            }
            return NONE;
        default:
            return NONE;
    }
}

int decideWhetherToTakeLoan(GameState *game, int playerIndex) {
    PlayerName player = game->players[playerIndex].name;

    switch (player) {
        case AGGRESSIVE_INVESTOR: {
            int unownedCount = 0;
            for (int i = 0; i < 22; i++) {
                if (game->properties[i].owner == Bank) {
                    unownedCount++;
                }
            }
            return unownedCount > 0;
        }
        case CONSERVATIVE_BANKER:
            return game->players[playerIndex].cash < 1000;
        case RISK_TAKER:
            return 1;
        case OPPORTUNISTIC_TRADER:
            return game->economy.marketBoomGroup >= 0 && 
                   game->currentRound < game->economy.marketBoomExpiryRound;
        default:
            return 0;
    }
}

int decideWhetherToRepay(GameState *game, int playerIndex) {
    PlayerName strategy = game->players[playerIndex].name;

    switch (strategy) {
        case AGGRESSIVE_INVESTOR:
            return game->players[playerIndex].cash > (game->players[playerIndex].loan.amount * 2);
        case CONSERVATIVE_BANKER:
            return game->players[playerIndex].cash >= game->players[playerIndex].loan.amount;
        case RISK_TAKER:
            return 0; //assumes doesn't repay voluntarily
        case OPPORTUNISTIC_TRADER:
            return 0; //assumes doesn't repay voluntarily
        default:
            return 0;
    }
}

