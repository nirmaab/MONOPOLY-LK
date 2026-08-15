#include <stdio.h>
#include "stdlib.h"

#include "types.h"
#include "functions.h"

int main() {
    GameState game;

    printf("============================================\n");
    printf("MONOPOLY-LK Simulation\n");
    printf("============================================\n\n");
    
    printf("Player 1 : Aggressive Investor\n");
    printf("Player 2 : Conservative Banker\n");
    printf("Player 3 : Risk Taker\n");
    printf("Player 4 : Opportunistic Trader\n\n");
    printf("Each player begins with LKR 30,000.\n\n");

    initBoard(&game);
    initPlayers(&game);

    srand(1); 
    determineTurnOrder(&game);

    runGame(&game);

    return 0;
}
