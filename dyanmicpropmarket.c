#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "functions.h"

const char* groupName(PropertyGroup group) {
    switch (group) {
        case BROWN: return "Brown";
        case LIGHT_BLUE: return "Light Blue";
        case PINK: return "Pink";
        case ORANGE: return "Orange";
        case RED: return "Red";
        case YELLOW: return "Yellow";
        case GREEN: return "Green";
        case DARK_BLUE: return "Dark Blue";
        default: return "Unknown";
    }
}


PropertyGroup pickEligibleGroup(GameState *game, int currentRound, PropertyGroup exclude) {
    PropertyGroup candidate;
    int eligible = 0;

    while (!eligible) {
        candidate = rand() % 8;

        if (candidate == exclude) {
            continue;
        }

        if (currentRound - game->lastAffectedRound[candidate] >= 30) {
            eligible = 1;
        }
    }

    return candidate;
}

void updatePropertyMarket(GameState *game, int currentRound) {
    PropertyGroup boomGroup = pickEligibleGroup(game, currentRound, -1);
    PropertyGroup declineGroup = pickEligibleGroup(game, currentRound, boomGroup);

    game->economy.marketBoomGroup = boomGroup;
    game->economy.marketBoomExpiryRound = currentRound + 10;
    game->lastAffectedRound[boomGroup] = currentRound;

    game->economy.marketDeclineGroup = declineGroup;
    game->economy.marketDeclineExpiryRound = currentRound + 10;
    game->lastAffectedRound[declineGroup] = currentRound;

    printf("\nDynamic Property Market\n");
    printf("Market Boom : %s\n", groupName(boomGroup));
    printf("Market Decline : %s\n", groupName(declineGroup));
}

void printMarketConditions(GameState *game) {
    printf("============================================\n");
    printf("Current Market Conditions\n");
    printf("============================================\n\n");

    if (game->currentRound < game->economy.marketBoomExpiryRound) {
        printf("Market Boom\n-------------\n");
        printf("%s (+20%%)\n", groupName(game->economy.marketBoomGroup));
        printf("Rounds Remaining : %d\n\n", game->economy.marketBoomExpiryRound - game->currentRound);
    }

    if (game->currentRound < game->economy.marketDeclineExpiryRound) {
        printf("Market Decline\n----------------\n");
        printf("%s (-15%%)\n", groupName(game->economy.marketDeclineGroup));
        printf("Rounds Remaining : %d\n\n", game->economy.marketDeclineExpiryRound - game->currentRound);
    }

    if (game->currentRound < game->economy.regionalExpiryRound) {
        printf("Regional Development\n-------------------------\n");
        printf("(%+d%%)\n", game->economy.regionalPercent);
        printf("Rounds Remaining : %d\n\n", game->economy.regionalExpiryRound - game->currentRound);
    }

    printf("Inflation\n------------\n");
    printf("%+d%%\n\n", game->economy.lastInflationRate);

    printf("Current Loan Interest\n------------------------\n");
    printf("%d%%\n\n", game->economy.currentInterestRate);
}
