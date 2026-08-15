#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "functions.h"

int getSquarePrice(GameState *game, SquareType type, int boardIndex) {
    if (type == PROPERTY) {
        return findProperty(game, boardIndex)->purchasePrice;
    } else if (type == RAILWAY) {
        return findRailway(game, boardIndex)->purchasePrice;
    } else if (type == UTILITY) {
        return findUtility(game, boardIndex)->purchasePrice;
    }
    return 0;
}

void setSquareOwner(GameState *game, SquareType type, int boardIndex, PlayerName owner) {
    if (type == PROPERTY) {
        findProperty(game, boardIndex)->owner = owner;
    } else if (type == RAILWAY) {
        findRailway(game, boardIndex)->owner = owner;
    } else if (type == UTILITY) {
        findUtility(game, boardIndex)->owner = owner;
    }
}
void startAuction(GameState *game, SquareType type, int boardIndex) {
    int basePrice = getSquarePrice(game, type, boardIndex);
    int currentBid = basePrice / 2;
    int activeInAuction[4];
    int highestBidder = -1;
    int activeCount = 0;

    for (int i = 0; i < 4; i++) {
        if (game->players[i].isBankrupt) {
            activeInAuction[i] = 0;
        } else {
            activeInAuction[i] = 1;
            activeCount++;
        }
    }

    printf("\nAuction Started.\n\n");
    printf("Property :\n%s\n\n", game->board[boardIndex].name);
    printf("Opening Bid :\nLKR %d.\n\n", currentBid);

    while (activeCount > 1) {
        for (int i = 0; i < 4; i++) {
            if (!activeInAuction[i]) continue;

            if (decideAuctionBid(game, i, basePrice, currentBid)) {
                currentBid += 250;
                highestBidder = i;
                printf("%s bids LKR %d.\n", playerName(game->players[i].name), currentBid);
            } else {
                activeInAuction[i] = 0;
                activeCount--;
                printf("%s withdraws.\n", playerName(game->players[i].name));
            }

            if (activeCount <= 1) break;
        }
    }

    if (highestBidder == -1) {
        printf("No one bid. Ownership remains with the Bank.\n");
        return;
    }

    game->players[highestBidder].cash -= currentBid;
    setSquareOwner(game, type, boardIndex, game->players[highestBidder].name);
    printf("%s wins the auction.\n", playerName(game->players[highestBidder].name));
}
