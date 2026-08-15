#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "functions.h"

void purchaseInsurance(GameState *game, int playerIndex, int boardIndex, InsurancePolicy policyType) {
    PropertySquare *property = findProperty(game, boardIndex);

    if (property->owner != game->players[playerIndex].name) {
        printf("%s does not own %s.\n", playerName(game->players[playerIndex].name), game->board[boardIndex].name);
        return;
    }

    if (property->insurancePolicy != NONE) {
        printf("%s is already insured.\n", game->board[boardIndex].name);
        return;
    }

    if (policyType == BUSINESS_INTERRUPTION && property->numBuildings != 5) {
        printf("Business Interruption insurance is only available for hotels.\n");
        return;
    }

    int value = currentPropertyValue(game, property);
    int premium;

    if (policyType == BASIC) {
        premium = value * 5 / 100;
    } else if (policyType == COMPREHENSIVE) {
        premium = value * 10 / 100;
    } else if (policyType == BUSINESS_INTERRUPTION) {
        premium = value * 15 / 100;
    } else {
        return;
    }

    if (hasActiveCard(&game->players[playerIndex], 16, game->currentRound)) {
        premium = premium * 80 / 100;
    }

    premium = premium * game->economy.insurancePremiumMultiplier / 100;

    if (game->players[playerIndex].cash < premium) {
        printf("%s cannot afford insurance on %s.\n", playerName(game->players[playerIndex].name), game->board[boardIndex].name);
        return;
    }

    game->players[playerIndex].cash -= premium;
    property->insurancePolicy = policyType;
    property->insuranceExpiryRound = game->currentRound + 20;

    const char *policyName;
    if (policyType == BASIC) {
        policyName = "Basic";
    } else if (policyType == COMPREHENSIVE) {
        policyName = "Comprehensive";
    } else {
        policyName = "Business Interruption";
    }

    printf("\n%s Insurance purchased.\n", policyName);
    printf("Property : %s\n", game->board[boardIndex].name);
    printf("Premium : LKR %d.\n", premium);
}

void checkInsuranceExpiry(GameState *game) {
    for (int i = 0; i < 22; i++) {
        if (game->properties[i].insurancePolicy != NONE) {
            if (game->currentRound == game->properties[i].insuranceExpiryRound - 3) {
                printf("Insurance policy on %s expires in 3 rounds.\n", game->board[game->properties[i].boardIndex].name);
            }
            if (game->currentRound >= game->properties[i].insuranceExpiryRound) {
                game->properties[i].insurancePolicy = NONE;
                printf("Insurance policy on %s has expired.\n", game->board[game->properties[i].boardIndex].name);
            }
        }
    }
}

void triggerDisaster(GameState *game, int playerIndex) {
    int developedIndices[22];
    int count = 0;

    for (int i = 0; i < 22; i++) {
        if (game->properties[i].numBuildings > 0) {
            if (playerIndex == -1 || game->properties[i].owner == game->players[playerIndex].name) {
                developedIndices[count] = i;
                count++;
            }
        }
    }

    if (count == 0) {
        printf("No developed properties to affect.\n");
        return;
    }

    int randomPick = rand() % count;
    int propertyIndex = developedIndices[randomPick];
    PropertySquare *property = &game->properties[propertyIndex];

    property->disasterDamaged = 1;

    const char *disasterNames[5] = {"Fire", "Flood", "Riot", "Building Collapse", "Electrical Failure"};
    const char *disasterName = disasterNames[rand() % 5];

    printf("%s occurred.\n", disasterName);
    printf("Affected Property :\n%s.\n", game->board[property->boardIndex].name);

    int repairCost;
    if (property->numBuildings == 5) {
        repairCost = property->hotelCost;
    } else {
        repairCost = property->houseCost * property->numBuildings;
    }

    int ownerIndex = -1;
    for (int i = 0; i < 4; i++) {
        if (game->players[i].name == property->owner) {
            ownerIndex = i;
            break;
        }
    }

    if (property->insurancePolicy == NONE) {
        if (game->players[ownerIndex].cash >= repairCost) {
            game->players[ownerIndex].cash -= repairCost;
            printf("No insurance. %s paid the full repair cost.\n\n", playerName(property->owner));
            printf("Repair Cost :\nLKR %d.\n", repairCost);
        } else {
            game->players[ownerIndex].hasExperiencedLoss = 1;
            printf("%s cannot afford repairs yet. Property remains damaged.\n", playerName(property->owner));
            return;
        }
    } else {
        int compensation;
        if (property->insurancePolicy == BASIC) {
            compensation = repairCost * 80 / 100;
        } else {
            compensation = repairCost;
        }

        game->players[ownerIndex].cash += compensation;
        game->players[ownerIndex].cash -= repairCost;

        printf("Insurance Claim Approved.\n\n");
        printf("Compensation Paid :\nLKR %d.\n\n", compensation);
    }

    property->disasterDamaged = 0;
}

void autoRepairDamagedProperties(GameState *game) {
    for (int i = 0; i < 22; i++) {
        if (game->properties[i].disasterDamaged) {
            int ownerIndex = -1;
            for (int p = 0; p < 4; p++) {
                if (game->players[p].name == game->properties[i].owner) {
                    ownerIndex = p;
                    break;
                }
            }
            if (ownerIndex == -1) continue;

            int repairCost;
            if (game->properties[i].numBuildings == 5) {
                repairCost = game->properties[i].hotelCost;
            } else {
                repairCost = game->properties[i].houseCost * game->properties[i].numBuildings;
            }

            if (game->players[ownerIndex].cash >= repairCost) {
                game->players[ownerIndex].cash -= repairCost;
                game->properties[i].disasterDamaged = 0;
                printf("%s repaired %s automatically.\n", 
                       playerName(game->players[ownerIndex].name), 
                       game->board[game->properties[i].boardIndex].name);
            }
        }
    }
}

