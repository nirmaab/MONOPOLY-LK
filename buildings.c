#include <stdio.h>

#include "types.h"
#include "functions.h"

int hasMonopoly(GameState *game, PlayerName player, PropertyGroup group) {
    for (int i = 0; i < 22; i++) {
        if (game->properties[i].group == group && game->properties[i].owner != player) {
            return 0; 
        }
    }
    return 1; 
}

int canBuildEvenly(GameState *game, PropertyGroup group, int boardIndex) {
    PropertySquare *target = findProperty(game, boardIndex);

    for (int i = 0; i < 22; i++) {
        if (game->properties[i].group == group && 
            game->properties[i].numBuildings < target->numBuildings) {
            return 0; 
        }
    }
    return 1;
}

void buildHouse(GameState *game, int playerIndex, int boardIndex) {
    PropertySquare *prop = findProperty(game, boardIndex);
    PlayerName player = game->players[playerIndex].name;

    if (!decideWhetherToBuild(game, playerIndex, boardIndex)) {
        return;
    } //Player Strategy

    if (hasActiveCard(&game->players[playerIndex], 14, game->currentRound)) {
        printf("%s cannot build - construction suspended (Labour Strike).\n", playerName(player));
        return;
    } //National Event Card-Labour Strike

    if (!hasMonopoly(game, player, prop->group)) {
        printf("%s does not have a monopoly on this group.\n", playerName(player));
        return;
    }

    if (prop->numBuildings >= 4) {
        printf("%s already has the max houses on %s.\n", playerName(player), 
                game->board[boardIndex].name);
        return;
    }

    if (!canBuildEvenly(game, prop->group, boardIndex)) {
        printf("Must build evenly across the group first.\n");
        return;
    }

    int cost = prop->houseCost;
    if (hasActiveCard(&game->players[playerIndex], 18, game->currentRound)) { 
        cost = cost * 110 / 100;
    } //Currency Depreciation-national event cards
    if (hasActiveCard(&game->players[playerIndex], 5, game->currentRound)) { 
        cost = cost * 70 / 100;
    } //Housing Subsidy-national event cards

    if (game->players[playerIndex].cash < cost) {
        printf("%s cannot afford a house on %s.\n", playerName(player), 
                game->board[boardIndex].name);
        return;
    }
    
    game->players[playerIndex].cash -= cost;
    prop->numBuildings++;

    printf("\n%s constructed one house on %s.\n", playerName(player), 
            game->board[boardIndex].name);
    printf("Construction Cost : LKR %d.\n", cost);
}

void buildHotel(GameState *game, int playerIndex, int boardIndex) {
    PropertySquare *prop = findProperty(game, boardIndex);
    PlayerName player = game->players[playerIndex].name;

    if (!decideWhetherToBuild(game, playerIndex, boardIndex)) {
        return;
    } //Player Strategy

    if (hasActiveCard(&game->players[playerIndex], 14, game->currentRound)) {
        printf("%s cannot build - construction suspended (Labour Strike).\n", playerName(player));
        return;
    } //National Event Card-Labour Strike

    if (prop->numBuildings == 5) {
        printf("%s already has a hotel on %s.\n", playerName(player), 
                game->board[boardIndex].name);
        return;
    }

    if (prop->numBuildings != 4) {
        printf("%s needs 4 houses on %s before building a hotel.\n", playerName(player), 
                game->board[boardIndex].name);
        return;
    }

    int cost = prop->hotelCost;
    if (hasActiveCard(&game->players[playerIndex], 18, game->currentRound)) { 
        cost = cost * 110 / 100;
    } //Currency Depreciation-national event cards
    if (hasActiveCard(&game->players[playerIndex], 5, game->currentRound)) { 
        cost = cost * 70 / 100;
    } //Housing Subsidy-national event cards

    if (game->players[playerIndex].cash < cost) {
        printf("%s cannot afford a hotel on %s.\n", playerName(player), 
                game->board[boardIndex].name);
        return;
    }

    game->players[playerIndex].cash -= cost;
    prop->numBuildings = 5;

    printf("\n%s upgraded %s to a Hotel.\n", playerName(player), 
            game->board[boardIndex].name);
}

int calculateDepreciation(int propertyAge) {
    if (propertyAge <= 50) {
        return 0;
    }

    int roundsPastFifty = propertyAge - 50;
    int depreciationPercent = roundsPastFifty / 5;

    if (depreciationPercent > 30) {
        depreciationPercent = 30;
    }

    return depreciationPercent;
}

int currentPropertyValue(GameState *game, PropertySquare *prop) { //For temporary changes in property value
    int depreciation = calculateDepreciation(prop->propertyAge);
    int value = prop->purchasePrice - (prop->purchasePrice * depreciation / 100);

    int ownerIndex = -1;
    for (int i = 0; i < 4; i++) {
        if (game->players[i].name == prop->owner) {
            ownerIndex = i;
            break;
        }
    }
    if (ownerIndex != -1 && hasActiveCard(&game->players[ownerIndex], 4, game->currentRound)) {
        value = value * 110 / 100;
    } //Stock Market Rise-national event cards

    if (ownerIndex != -1 && hasActiveCard(&game->players[ownerIndex], 8, game->currentRound)) { 
        value = value * 85 / 100;
    } //Economic Downturn-national event cards

    if (prop->group == game->economy.revaluedGroup && game->currentRound < game->economy.revaluationExpiryRound) {
        value = value * 115 / 100;
    } //Property Revaluation-national event cards

    if (prop->numBuildings == 5 && ownerIndex != -1 && hasActiveCard(&game->players[ownerIndex], 11, game->currentRound)) {
        value = value * 115 / 100;
    } //Foreign Funding-national event cards

    if (prop->isDamaged) {
        value = value * 85 / 100;
    }

    if (prop->group == game->economy.marketBoomGroup && game->currentRound < game->economy.marketBoomExpiryRound) {
        value = value * 120 / 100;
    } else if (prop->group == game->economy.marketDeclineGroup && game->currentRound < game->economy.marketDeclineExpiryRound) {
        value = value * 80 / 100;
    }

    for (int i = 0; i < game->economy.regionalAffectedCount; i++) {
        if (game->economy.regionalAffectedIndices[i] == prop->boardIndex && 
            game->currentRound < game->economy.regionalExpiryRound && 
            !game->economy.regionalIsRent) 
        {
            value = value + (value * game->economy.regionalPercent / 100);
            break;
        }
    }

    return value;
}

void incrementPropertyAges(GameState *game) {
    for (int i = 0; i < 22; i++) {
        game->properties[i].propertyAge++;
        game->properties[i].roundsSinceMaintenance++;

        if (game->properties[i].propertyAge > 50 && (game->properties[i].propertyAge - 50) % 5 == 0) {
            int depreciation = calculateDepreciation(game->properties[i].propertyAge);
            int currentValue = currentPropertyValue(game, &game->properties[i]);

            printf("Property\n%s\nhas depreciated by %d%%.\n\n", 
                   game->board[game->properties[i].boardIndex].name, depreciation);
            printf("Current Value\nLKR %d.\n", currentValue);
        }
    }
}

void renovateProperty(GameState *game, int playerIndex, int boardIndex) {
    PropertySquare *property = findProperty(game, boardIndex);
    PlayerName player = game->players[playerIndex].name;

    int currentValue = currentPropertyValue(game, property);
    int cost = currentValue * 10 / 100;

    if (game->players[playerIndex].cash < cost) {
        printf("%s cannot afford to renovate %s.\n", playerName(player), 
                game->board[boardIndex].name);
        return;
    }

    game->players[playerIndex].cash -= cost;
    property->propertyAge = 0;

    printf("%s renovated %s for LKR %d. Depreciation reset.\n",
           playerName(player), game->board[boardIndex].name, cost);
}

void decayBuildingConditions(GameState *game) {
    for (int i = 0; i < 22; i++) {
        if (game->properties[i].numBuildings > 0) {
            double newRating = game->properties[i].conditionRating * 0.98;
            game->properties[i].conditionRating = (int)newRating;

            if (game->properties[i].conditionRating < 0) {
                game->properties[i].conditionRating = 0;
            }
        }
    }
}

void maintainBuilding(GameState *game, int playerIndex, int boardIndex) { //Rule-LK 27
    PropertySquare *property = findProperty(game, boardIndex);

    int cost;
    if (property->numBuildings == 5) {
        cost = property->hotelCost * 8 / 100;
    } else {
        cost = (property->houseCost * 5 / 100) * property->numBuildings;
    }

    if (property->isDamaged) {
        cost = cost * 150 / 100;
    }

    if (game->players[playerIndex].cash < cost) {
        printf("%s cannot afford maintenance on %s.\n",
               playerName(game->players[playerIndex].name), game->board[boardIndex].name);
        return;
    }

    game->players[playerIndex].cash -= cost;
    property->conditionRating = 100;
    property->roundsSinceMaintenance = 0;

    printf("%s performed maintenance on %s.\n\n", 
       playerName(game->players[playerIndex].name), game->board[boardIndex].name);
    printf("Maintenance Cost : LKR %d.\n\n", cost);
    printf("Condition restored to 100%%.\n");    
}

void checkStructuralDamage(GameState *game) { //Rule-LK 28
    for (int i = 0; i < 22; i++) {
        if (game->properties[i].numBuildings > 0) {
            game->properties[i].roundsSinceMaintenance++;

            if (game->properties[i].roundsSinceMaintenance >= 20 && 
                !game->properties[i].isDamaged) 
            {
                game->properties[i].isDamaged = 1;
                printf("%s has suffered structural damage from neglect.\n",
                       game->board[game->properties[i].boardIndex].name);
            }
        }
    }
}

void repairDamage(GameState *game, int playerIndex, int boardIndex) { //Rule-LK 29
    PropertySquare *property = findProperty(game, boardIndex);

    if (!property->isDamaged) {
        printf("%s is not damaged.\n", game->board[boardIndex].name);
        return;
    }

    int replacementCost;
    if (property->numBuildings == 5) {
        replacementCost = property->hotelCost;
    } 
    else {
        replacementCost = property->houseCost * property->numBuildings;
    }
    int cost = replacementCost * 25 / 100;

    if (game->players[playerIndex].cash < cost) {
        printf("%s cannot afford to repair %s.\n",
               playerName(game->players[playerIndex].name), 
               game->board[boardIndex].name);
        return;
    }

    game->players[playerIndex].cash -= cost;
    property->conditionRating = 100;
    property->roundsSinceMaintenance = 0;
    property->isDamaged = 0;

    printf("%s repaired %s.\n\n", 
       playerName(game->players[playerIndex].name), game->board[boardIndex].name);
    printf("Repair Cost : LKR %d.\n\n", cost);
    printf("Damage cleared.\n");
}
