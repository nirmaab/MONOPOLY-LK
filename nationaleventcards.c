#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "functions.h"

void addActiveCard(Player *player, int cardId, int expiryRound) {
    if (player->activeCardCount < 5) {
        player->activeCardIds[player->activeCardCount] = cardId;
        player->activeCardExpiry[player->activeCardCount] = expiryRound;
        player->activeCardCount++;
    }
}

int hasActiveCard(Player *player, int cardId, int currentRound) {
    for (int i = 0; i < player->activeCardCount; i++) {
        if (player->activeCardIds[i] == cardId && currentRound < player->activeCardExpiry[i]) {
            return 1;
        }
    }
    return 0;
}

void applyNationalEventCard(GameState *game, int playerIndex) {
    int card = game->nationaleventscard;

    switch (card) {
        case 0: // Tourism Hype
            addActiveCard(&game->players[playerIndex], 0, game->currentRound + 5);
            printf("National Event Card: Tourism Hype. %s's hotels earn double rent for 5 rounds.\n",
                playerName(game->players[playerIndex].name));
            break;
        
        case 1: // Fuel Shortage
            addActiveCard(&game->players[playerIndex], 1, game->currentRound + 5);
            printf("National Event Card: Fuel Shortage. %s's railway rent doubles for 5 rounds.\n",
                playerName(game->players[playerIndex].name));
            break;

        case 2: // Heavy Floods
            {
                int coastalIndices[3] = {26, 27, 29};
                int candidates[3];
                int count = 0;

                for (int i = 0; i < 3; i++) {
                    PropertySquare *coastalProp = findProperty(game, coastalIndices[i]);
                    if (coastalProp->numBuildings > 0 && coastalProp->owner == game->players[playerIndex].name) {
                        candidates[count] = coastalIndices[i];
                        count++;
                    }
                }

                if (count == 0) {
                    printf("National Event Card: Heavy Floods. %s has no developed coastal properties.\n",
                        playerName(game->players[playerIndex].name));
                    break;
                }

                int pick = candidates[rand() % count];
                PropertySquare *property = findProperty(game, pick);
                property->disasterDamaged = 1;

                int repairCost;
                if (property->numBuildings == 5) {
                    repairCost = property->hotelCost;
                } else {
                    repairCost = property->houseCost * property->numBuildings;
                }

                printf("National Event Card: Heavy Floods. %s's coastal property %s damaged. Repair cost: LKR %d.\n",
                    playerName(game->players[playerIndex].name), game->board[pick].name, repairCost);

                if (property->insurancePolicy == NONE) {
                    if (game->players[playerIndex].cash >= repairCost) {
                        game->players[playerIndex].cash -= repairCost;
                        printf("%s was uninsured and paid the full repair cost.\n", playerName(game->players[playerIndex].name));
                    } else {
                        game->players[playerIndex].hasExperiencedLoss = 1;
                        printf("%s cannot afford repairs yet. %s remains damaged.\n", 
                            playerName(game->players[playerIndex].name), game->board[pick].name);
                        break; // break here since this is inside a switch-case, not 'return'
                    }
                } else {
                    int compensation;
                    if (property->insurancePolicy == BASIC) {
                        compensation = repairCost * 80 / 100;
                    } else {
                        compensation = repairCost;
                    }
                    game->players[playerIndex].cash += compensation;
                    game->players[playerIndex].cash -= repairCost;
                    printf("%s's insurance covered LKR %d of the repair cost.\n", playerName(game->players[playerIndex].name), compensation);
                }

                property->disasterDamaged = 0;
            }
            break;

        case 3: // Political Rally
            {
                int ownedIndices[22];
                int count = 0;

                for (int i = 0; i < 22; i++) {
                    if (game->properties[i].owner == game->players[playerIndex].name) {
                        ownedIndices[count] = i;
                        count++;
                    }
                }

                if (count == 0) {
                    printf("National Event Card: Political Rally. %s owns no properties.\n",
                        playerName(game->players[playerIndex].name));
                    break;
                }

                int pick = ownedIndices[rand() % count];
                game->properties[pick].closedUntilRound = game->currentRound + 2;

                printf("National Event Card: Political Rally. %s's property %s is closed for 2 rounds.\n",
                    playerName(game->players[playerIndex].name), game->board[game->properties[pick].boardIndex].name);
            }
            break;

        case 4: // Stock Market 
            addActiveCard(&game->players[playerIndex], 4, game->currentRound + 15);
            printf("National Event Card: Stock Market Rise. %s's property values increase 10%% for 15 rounds.\n",
                playerName(game->players[playerIndex].name));
            break;

        case 5: // Housing Subsidy
            addActiveCard(&game->players[playerIndex], 5, game->currentRound + 15);
            printf("National Event Card: Housing Subsidy. %s's construction costs decrease 30%% for 15 rounds.\n",
                playerName(game->players[playerIndex].name));
            break;

        case 6: // Interest Rate Cut
            addActiveCard(&game->players[playerIndex], 6, game->currentRound + 15);
            printf("National Event Card: Interest Rate Cut. %s's loan interest reduced 2%% for 15 rounds.\n",
                playerName(game->players[playerIndex].name));
            break;
        case 7: // Interest Rate Increase
            addActiveCard(&game->players[playerIndex], 7, game->currentRound + 15);
            printf("National Event Card: Interest Rate Increase. %s's loan interest increased 2%% for 15 rounds.\n",
                playerName(game->players[playerIndex].name));
            break;

        case 8: // Economic Downturn
            addActiveCard(&game->players[playerIndex], 8, game->currentRound + 15);
            printf("National Event Card: Economic Downturn. %s's property values decrease 15%% for 15 rounds.\n",
                playerName(game->players[playerIndex].name));
            break;

        case 9: // Tax Amnesty
            for (int i = 0; i < 4; i++) {
                game->players[i].cash += 2000;
            }
            printf("National Event Card: Tax Amnesty. Each player receives LKR 2,000.\n");
            break;

        case 10: // Power Failure
            addActiveCard(&game->players[playerIndex], 10, game->currentRound + 3);
            printf("National Event Card: Power Failure. %s's utility income halved for 3 rounds.\n",
                playerName(game->players[playerIndex].name));
            break;

        case 11: // Foreign Funding
            addActiveCard(&game->players[playerIndex], 11, game->currentRound + 15);
            printf("National Event Card: Foreign Funding. %s's hotel values increase 15%% for 15 rounds.\n",
                playerName(game->players[playerIndex].name));
            break;

        case 12: // Port Expansion
            addActiveCard(&game->players[playerIndex], 12, game->currentRound + 15);
            printf("National Event Card: Port Expansion. %s's railway station purchase prices increase 20%% for 15 rounds.\n",
                playerName(game->players[playerIndex].name));
            break;

        case 13: // Festival Season
            addActiveCard(&game->players[playerIndex], 13, game->currentRound + 15);
            printf("National Event Card: Festival Season. %s's hotels receive 50%% additional rent for 15 rounds.\n",
                playerName(game->players[playerIndex].name));
            break;

        case 14: // Labour Strike
            addActiveCard(&game->players[playerIndex], 14, game->currentRound + 2);
            printf("National Event Card: Labour Strike. %s's construction suspended for 2 rounds.\n",
                playerName(game->players[playerIndex].name));
            break;

        case 15: // Government Grant
            {
                int randomPlayer = rand() % 4;
                game->players[randomPlayer].cash += 5000;
                printf("National Event Card: Government Grant. %s receives LKR 5,000.\n",
                    playerName(game->players[randomPlayer].name));
            }
            break;

        case 16: // Insurance Discount
            addActiveCard(&game->players[playerIndex], 16, game->currentRound + 15);
            printf("National Event Card: Insurance Discount. %s's premiums decrease 20%% for 15 rounds.\n",
                playerName(game->players[playerIndex].name));
            break;

        case 17: // Property Revaluation
            game->economy.revaluedGroup = pickEligibleGroup(game, game->currentRound, -1);
            game->economy.revaluationExpiryRound = game->currentRound + 15;
            printf("National Event Card: Property Revaluation. %s group appreciates 15%% for 15 rounds.\n",
                groupName(game->economy.revaluedGroup));
            break;

        case 18: // Currency Depreciation
            addActiveCard(&game->players[playerIndex], 18, game->currentRound + 15);
            printf("National Event Card: Currency Depreciation. %s's construction costs increase 10%% for 15 rounds.\n",
                playerName(game->players[playerIndex].name));
            break;

        case 19: // National Disaster
            triggerDisaster(game, playerIndex);
            break;

        default:
            break;
    }

    game->nationaleventscard++;
}
