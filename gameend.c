#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "functions.h"

int calculateNetWorth(GameState *game, int playerIndex) {
    PlayerName player = game->players[playerIndex].name;
    int netWorth = game->players[playerIndex].cash;

    for (int i = 0; i < 22; i++) {
        if (game->properties[i].owner == player) {
            netWorth += currentPropertyValue(game, &game->properties[i]);

            int buildingValue = 0;
            if (game->properties[i].numBuildings == 5) {
                buildingValue = game->properties[i].hotelCost;
            } else {
                buildingValue = game->properties[i].houseCost * game->properties[i].numBuildings;
            }
            netWorth += buildingValue;
        }
    }

    for (int i = 0; i < 4; i++) {
        if (game->railways[i].owner == player) {
            netWorth += game->railways[i].purchasePrice;
        }
    }

    for (int i = 0; i < 2; i++) {
        if (game->utilities[i].owner == player) {
            netWorth += game->utilities[i].purchasePrice;
        }
    }

    netWorth -= game->players[playerIndex].loan.amount;

    return netWorth;
}

void tryToAvoidBankruptcy(GameState *game, int playerIndex, int amountNeeded) {
    PlayerName player = game->players[playerIndex].name;

    while (game->players[playerIndex].cash < amountNeeded) {
        int mortgaged = 0;

        for (int i = 0; i < 22; i++) {
            if (game->properties[i].owner == player && !game->properties[i].isMortgaged) {
                mortgageProperty(game, playerIndex, game->properties[i].boardIndex);
                mortgaged = 1;
                break;
            }
        }
        if (!mortgaged) {
            for (int i = 0; i < 4; i++) {
                if (game->railways[i].owner == player && !game->railways[i].isMortgaged) {
                    mortgageRailway(game, playerIndex, game->railways[i].boardIndex);
                    mortgaged = 1;
                    break;
                }
            }
        }
        if (!mortgaged) {
            for (int i = 0; i < 2; i++) {
                if (game->utilities[i].owner == player && !game->utilities[i].isMortgaged) {
                    mortgageUtility(game, playerIndex, game->utilities[i].boardIndex);
                    mortgaged = 1;
                    break;
                }
            }
        }

        if (!mortgaged) {
            break; // nothing left to mortgage
        }
    }
}

void declareBankruptcy(GameState *game, int playerIndex) {
    PlayerName player = game->players[playerIndex].name;

    for (int i = 0; i < 22; i++) {
        if (game->properties[i].owner == player) {
            game->properties[i].numBuildings = 0;
            game->properties[i].insurancePolicy = NONE;
            game->properties[i].owner = Bank;
            startAuction(game, PROPERTY, game->properties[i].boardIndex);
        }
    }

    for (int i = 0; i < 4; i++) {
        if (game->railways[i].owner == player) {
            game->railways[i].owner = Bank;
            startAuction(game, RAILWAY, game->railways[i].boardIndex);
        }
    }

    for (int i = 0; i < 2; i++) {
        if (game->utilities[i].owner == player) {
            game->utilities[i].owner = Bank;
            startAuction(game, UTILITY, game->utilities[i].boardIndex);
        }
    }

    game->players[playerIndex].loan.amount = 0;
    game->players[playerIndex].loan.interestRate = 0;
    game->players[playerIndex].loan.roundsRemaining = 0;
    game->players[playerIndex].isBankrupt = 1;

    printf("\n%s has been declared bankrupt.\n", playerName(player));
    printf("Remaining assets transferred to the Bank.\n");
}

void attemptPayment(GameState *game, int playerIndex, int amount) {
    if (game->players[playerIndex].cash < amount) {
        tryToAvoidBankruptcy(game, playerIndex, amount);
    }

    if (game->players[playerIndex].cash < amount) {
        declareBankruptcy(game, playerIndex);
        return;
    }

    game->players[playerIndex].cash -= amount;
}

int checkGameEnd(GameState *game) { //All bankrupt except 1 player
    int solventCount = 0;
    int lastSolvent = -1;

    for (int i = 0; i < 4; i++) {
        if (!game->players[i].isBankrupt) {
            solventCount++;
            lastSolvent = i;
        }
    }

    if (solventCount <= 1) {
        printf("============================================\n");
        printf("GAME OVER\n");
        printf("============================================\n\n");

        if (lastSolvent != -1) {
            int netWorth = calculateNetWorth(game, lastSolvent);
            int totalCash = game->players[lastSolvent].cash;
            int totalPropertyValue = netWorth - totalCash + game->players[lastSolvent].loan.amount;

            printf("Winner:\n\n%s\n\n", playerName(game->players[lastSolvent].name));
            printf("Total Cash:\n\nLKR %d\n\n", totalCash);
            printf("Total Property Value:\n\nLKR %d\n\n", totalPropertyValue);

            if (game->players[lastSolvent].loan.amount > 0) {
                printf("Outstanding Loans:\n\nLKR %d\n\n", game->players[lastSolvent].loan.amount);
            } else {
                printf("Outstanding Loans:\n\nNone\n\n");
            }

            printf("Net Worth:\n\nLKR %d\n", netWorth);
        } else {
            printf("All players are bankrupt. No winner.\n");
        }
        return 1;
    }

    return 0;
}

void declareWinnerByNetWorth(GameState *game) {
    int bestIndex = -1;
    int bestNetWorth = -1;

    for (int i = 0; i < 4; i++) {
        if (!game->players[i].isBankrupt) {
            int netWorth = calculateNetWorth(game, i);
            if (netWorth > bestNetWorth) {
                bestNetWorth = netWorth;
                bestIndex = i;
            }
        }
    }

    printf("============================================\n");
    printf("GAME OVER\n");
    printf("============================================\n\n");

    if (bestIndex == -1) {
        printf("All players bankrupt. No winner.\n");
        return;
    }

    int totalCash = game->players[bestIndex].cash;
    int totalPropertyValue = bestNetWorth - totalCash + game->players[bestIndex].loan.amount;

    printf("Winner\n\n%s\n\n", playerName(game->players[bestIndex].name));
    printf("Total Cash\n\nLKR %d\n\n", totalCash);
    printf("Total Property Value\n\nLKR %d\n\n", totalPropertyValue);

    if (game->players[bestIndex].loan.amount > 0) {
        printf("Outstanding Loans\n\nLKR %d\n\n", game->players[bestIndex].loan.amount);
    } else {
        printf("Outstanding Loans\n\nNone\n\n");
    }

    printf("Net Worth\n\nLKR %d\n", bestNetWorth);
}
