#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "functions.h"


void applyInflation(GameState *game) {
    int inflationRates[6] = {-3, 0, 2, 5, 8, 12};
    int rate = inflationRates[rand() % 6];

    printf("Inflation event: %d%%\n", rate);
    game->economy.lastInflationRound = game->currentRound;
    game->economy.lastInflationRate = rate;

    for (int i = 0; i < 22; i++) {
        game->properties[i].purchasePrice += game->properties[i].purchasePrice * rate / 100;
        game->properties[i].houseCost += game->properties[i].houseCost * rate / 100;
        game->properties[i].hotelCost += game->properties[i].hotelCost * rate / 100;
        game->properties[i].baseRent += game->properties[i].baseRent * rate / 100;
        game->properties[i].mortgageValue += game->properties[i].mortgageValue * rate / 100;
    }

    for (int i = 0; i < 4; i++) {
        game->railways[i].purchasePrice += game->railways[i].purchasePrice * rate / 100;
        game->railways[i].mortgageValue += game->railways[i].mortgageValue * rate / 100;
    }

    for (int i = 0; i < 2; i++) {
        game->utilities[i].purchasePrice += game->utilities[i].purchasePrice * rate / 100;
        game->utilities[i].mortgageValue += game->utilities[i].mortgageValue * rate / 100;
    }

    game->economy.currentInterestRate += game->economy.currentInterestRate * rate / 100;
}
