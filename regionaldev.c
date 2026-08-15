#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "functions.h"

RegionalCard regionalCards[8] = {
    {{26, 27, 29}, 3, 40, 1},   // Southern Tourism Boom: rent +40%
    {{13, 11, 14}, 3, 20, 0},   // IT Industry Growth: value +20%
    {{31, 32, 34}, 3, 30, 0},   // Northern Development Programme: value +30%
    {{37}, 1, 35, 0},           // Tea Export Boom: value +35%
    {{16, 18, 19}, 3, 30, 1},   // Airport Expansion: rent +30%
    {{23, 21}, 2, 20, 0},       // University City Growth: value +20%
    {{26, 27, 29}, 3, -30, 1},  // Beach Pollution: rent -30%
    {{8, 9}, 2, -20, 0},        // Flood Damage: value -20%
};

void updateRegionalDevelopment(GameState *game, int currentRound) {
    int pick = rand() % 8;
    RegionalCard card = regionalCards[pick];

    game->economy.regionalAffectedCount = card.count;
    for (int i = 0; i < card.count; i++) {
        game->economy.regionalAffectedIndices[i] = card.indices[i];
    }
    game->economy.regionalPercent = card.percent;
    game->economy.regionalIsRent = card.isRent;
    game->economy.regionalExpiryRound = currentRound + 15;

    const char *effectType;
    if (card.isRent) {
        effectType = "rent";
    } else {
        effectType = "value";
    }

    printf("Regional Development Card\n\n");
    printf("Affected Properties :\n");
    for (int i = 0; i < card.count; i++) {
        printf("%s\n", game->board[card.indices[i]].name);
    }
    printf("\n%d%% %s for 15 rounds.\n", card.percent, effectType);
}
