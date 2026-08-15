#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "functions.h"

void initPlayers(GameState *game) {
    Player players[4] = {
        {AGGRESSIVE_INVESTOR,  30000, 0, 0, 0, {0,0,0,{0},0}, 0, {0,0,0,0,0}, {0,0,0,0,0}, 0, 0, 0},
        {CONSERVATIVE_BANKER,  30000, 0, 0, 0, {0,0,0,{0},0}, 0, {0,0,0,0,0}, {0,0,0,0,0}, 0, 0, 0},
        {RISK_TAKER,           30000, 0, 0, 0, {0,0,0,{0},0}, 0, {0,0,0,0,0}, {0,0,0,0,0}, 0, 0, 0},
        {OPPORTUNISTIC_TRADER, 30000, 0, 0, 0, {0,0,0,{0},0}, 0, {0,0,0,0,0}, {0,0,0,0,0}, 0, 0, 0},
    };

    for (int i = 0; i < 4; i++) {
        game->players[i] = players[i];
    }
}

const char* playerName(PlayerName p) {
    switch (p) {
        case Bank: return "Bank";
        case AGGRESSIVE_INVESTOR: return "Aggressive Investor";
        case CONSERVATIVE_BANKER: return "Conservative Banker";
        case RISK_TAKER: return "Risk Taker";
        case OPPORTUNISTIC_TRADER: return "Opportunistic Trader";
        default: return "Unknown";
    }
}

void movePlayer(int playerIndex, int diceVal, GameState *game) {
    int oldPosition = game->players[playerIndex].position;
    game->players[playerIndex].position = (oldPosition + diceVal) % 40;

    printf("%s moves from Square %d to Square %d.\n",
           playerName(game->players[playerIndex].name), oldPosition, game->players[playerIndex].position);

    if (game->players[playerIndex].position < oldPosition) {
        game->players[playerIndex].cash += 2000;
        game->players[playerIndex].lapsCompleted++;

        printf("%s passed GO.\n", playerName(game->players[playerIndex].name));
        printf("Collected LKR 2,000.\n");
        printf("Current Balance : LKR %d.\n", game->players[playerIndex].cash);

        if (game->players[playerIndex].loan.amount > 0) {
            game->players[playerIndex].loan.roundsRemaining--;
            int interest = (game->players[playerIndex].loan.amount *
                             game->players[playerIndex].loan.interestRate) / 100;
            game->players[playerIndex].loan.amount += interest;

            if (game->players[playerIndex].loan.roundsRemaining <= 0) {
                foreclose(game, playerIndex);
            }
        } /*Added here, since the the loan interest related player laps, 
        instead of all 4 players round like in property age....*/
    }
}

void resolveJail(GameState *game, int playerIndex) {
    if (!game->players[playerIndex].inJail) {
        return;
    }

    int die1 = rand() % 6 + 1;
    int die2 = rand() % 6 + 1;

    if (die1 == die2) {
        printf("%s rolled doubles (%d, %d) and left jail.\n",
               playerName(game->players[playerIndex].name), die1, die2);
        game->players[playerIndex].inJail = 0;
        game->players[playerIndex].jailTurns = 0;
        return;
    }

    if (game->players[playerIndex].cash >= 300) {
        game->players[playerIndex].cash -= 300;
        game->players[playerIndex].inJail = 0;
        game->players[playerIndex].jailTurns = 0;
        printf("%s paid LKR 300 bail and left jail.\n", playerName(game->players[playerIndex].name));
        return;
    }

    game->players[playerIndex].jailTurns++;
    if (game->players[playerIndex].jailTurns >= 3) {
        game->players[playerIndex].inJail = 0;
        game->players[playerIndex].jailTurns = 0;
        printf("%s served 3 turns and is released from jail.\n", playerName(game->players[playerIndex].name));
    } else {
        printf("%s remains in jail (turn %d).\n", playerName(game->players[playerIndex].name), game->players[playerIndex].jailTurns);
    }
}