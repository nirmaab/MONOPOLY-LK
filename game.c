#include <stdio.h>
#include "stdlib.h"

#include "types.h"
#include "functions.h"

int rollDice() {
    int diceVal;
    int lower = 2, upper = 12;

    diceVal = (rand() % (upper - lower + 1) + lower);

    return diceVal;
}

void determineTurnOrder(GameState *game) {
    int rolls[4];
    int tie;

    for (int i = 0; i < 4; i++) {
        rolls[i] = rollDice();
    }

    printf("============================================\n");
    printf("Determining the First Player\n");
    printf("============================================\n\n");

    for (int i = 0; i < 4; i++) {
        printf("%s rolls %d.\n", playerName(game->players[i].name), rolls[i]);
    }
    printf("\n");

    // Initial sort
    for (int i = 0; i < 4 - 1; i++) {
        for (int j = 0; j < 4 - 1 - i; j++) {
            if (rolls[j] < rolls[j + 1]) {
                int tempRoll = rolls[j];
                rolls[j] = rolls[j + 1];
                rolls[j + 1] = tempRoll;

                Player tempPlayer = game->players[j];
                game->players[j] = game->players[j + 1];
                game->players[j + 1] = tempPlayer;
            }
        }
    }

    do {
        tie = 0;

        for (int i = 0; i < 3; i++) {
            if (rolls[i] == rolls[i + 1]) {
                tie = 1;

                rolls[i] = rollDice();
                rolls[i + 1] = rollDice();

                printf("%s rolls %d.\n", playerName(game->players[i].name), rolls[i]);
                printf("%s rolls %d.\n", playerName(game->players[i + 1].name), rolls[i + 1]);

                if (rolls[i] < rolls[i + 1]) {
                    int tempRoll = rolls[i];
                    rolls[i] = rolls[i + 1];
                    rolls[i + 1] = tempRoll;

                    Player tempPlayer = game->players[i];
                    game->players[i] = game->players[i + 1];
                    game->players[i + 1] = tempPlayer;
                }

                break;
            }
        }

    } while (tie);

    printf("\n%s will begin the game.\n\n", playerName(game->players[0].name));
    printf("Turn order:\n");
    for (int i = 0; i < 4; i++) {
        printf("%s\n", playerName(game->players[i].name));
    }

    printf("\n============================================\n");
    printf("\n");
}

void resolveLanding(GameState *game, int playerIndex, int diceVal/*For utilityRent*/) {
    int position = game->players[playerIndex].position;
    SquareType type = game->board[position].type;

    switch (type) {
        case PROPERTY: {
            PropertySquare *property = findProperty(game, position);
            printf("%s landed on %s.\n", 
                playerName(game->players[playerIndex].name), 
                game->board[position].name);
            if (property->owner == Bank) {
                if (decideWhetherToBuyProperty(game, playerIndex, position)) {
                    purchaseProperty(game, playerIndex, position);
                } else {
                    printf("%s declined to purchase %s.\n", playerName(game->players[playerIndex].name), game->board[position].name);
                    startAuction(game, PROPERTY, position);
                }
            } else if (property->owner != game->players[playerIndex].name) {
                propertyRent(game, playerIndex, position);
            }
            break;
        }

        case RAILWAY: {
            RailwaySquare *railway = findRailway(game, position);
            printf("%s landed on %s.\n", 
                playerName(game->players[playerIndex].name), 
                game->board[position].name);
            if (railway->owner == Bank) {
                if (decideWhetherToBuyRailway(game, playerIndex, position)) {
                    purchaseRailway(game, playerIndex, position);
                } else {
                    printf("%s declined to purchase %s.\n", playerName(game->players[playerIndex].name), game->board[position].name);
                        startAuction(game, RAILWAY, position);
                }
            } else if (railway->owner != game->players[playerIndex].name) {
                railwayRent(game, playerIndex, position);
            }
            break;
        }

        case UTILITY: {
            UtilitySquare *utility = findUtility(game, position);
            printf("%s landed on %s.\n", 
                playerName(game->players[playerIndex].name), 
                game->board[position].name);
            if (utility->owner == Bank) {
                if (decideWhetherToBuyUtility(game, playerIndex, position)) {
                    purchaseUtility(game, playerIndex, position);
                } else {
                    printf("%s declined to purchase %s.\n", playerName(game->players[playerIndex].name), game->board[position].name);
                    startAuction(game, UTILITY, position);
                }
            } else if (utility->owner != game->players[playerIndex].name) {
                utilityRent(game, playerIndex, position, diceVal);
            }
            break;
        }

        case TAX:
            printf("%s landed on %s.\n\n", playerName(game->players[playerIndex].name), game->board[position].name);
            payAssetTax(game, playerIndex, game->economy.incomeTaxRate);
            break;

        case EVENT:
            printf("%s landed on %s.\n\n", playerName(game->players[playerIndex].name), game->board[position].name);
            if (position == 2) {
                payAssetTax(game, playerIndex, game->economy.communityFundTaxRate);
            } else {
                applyNationalEventCard(game, playerIndex);
            }
            break;

        case INSURANCE: {
            PlayerName player = game->players[playerIndex].name;
            printf("%s landed on %s.\n", playerName(player), game->board[position].name);

            int found = 0;
            for (int i = 0; i < 22; i++) {
                if (game->properties[i].owner == player && game->properties[i].insurancePolicy == NONE) {
                    InsurancePolicy chosen = decideInsurancePolicy(game, playerIndex, game->properties[i].boardIndex);
                    if (chosen != NONE) {
                        purchaseInsurance(game, playerIndex, game->properties[i].boardIndex, chosen);
                        found = 1;
                    }
                    break;
                }
            }

            if (!found) {
                printf("%s has no uninsured properties to insure (or declined).\n", playerName(player));
            }
            break;
        }

        case BANK: {
            PlayerName player = game->players[playerIndex].name;
            printf("%s landed on %s.\n", playerName(player), game->board[position].name);

            if (game->players[playerIndex].loan.amount > 0) {
                if (decideWhetherToRepay(game, playerIndex)) {
                    repayLoan(game, playerIndex); // full repayment only
                } else {
                    printf("%s chooses not to repay the loan right now.\n", playerName(player));
                }
            } else if (decideWhetherToTakeLoan(game, playerIndex)) {
                int loanAmount = maxLoanAmount(game, player); //Everyone takes maxLoanAmount
                if (loanAmount > 0) {
                    takeLoan(game, playerIndex, loanAmount);
                } else {
                    printf("%s has no eligible collateral for a loan.\n", playerName(player));
                }
            } else {
                printf("%s does not want a loan right now.\n", playerName(player));
            }
            break;
        }

        case SPECIAL: {
            if (position == 30) {
                printf("%s landed on Go To Jail!\n", playerName(game->players[playerIndex].name));
                game->players[playerIndex].position = 10;
                game->players[playerIndex].inJail = 1;
            } else {
                printf("%s landed on %s.\n", playerName(game->players[playerIndex].name), game->board[position].name);
            }
            break;
        }

        case START:
            printf("%s landed on GO.\n", playerName(game->players[playerIndex].name));
            break;
    }
}

void runGame(GameState *game) {
    int maxRounds = 500;
    int lastRound = 0;

    printf("Round %d:\n", 1);
    while (1) { 
        int currentRound = -1;

        for (int i = 0; i < 4; i++) {
            if (!game->players[i].isBankrupt) {
                if (currentRound == -1 || game->players[i].lapsCompleted < currentRound) {
                    currentRound = game->players[i].lapsCompleted;
                }
            }
        }

        game->currentRound = currentRound;

        if (checkGameEnd(game)) { //check if only 1 player standing
            break;
        }

        if (currentRound != lastRound) {
            //Round Summary
            printf("============================================\n");
            printf("Round %d Summary\n", currentRound);
            printf("============================================\n\n");
            for (int i = 0; i < 4; i++) {
                printf("%s\n\n", playerName(game->players[i].name));
                printf("Cash : LKR %d\n\n", game->players[i].cash);
                printf("Net Worth : LKR %d\n\n", calculateNetWorth(game, i));

                int propCount = 0, hotelCount = 0;
                for (int p = 0; p < 22; p++) {
                    if (game->properties[p].owner == game->players[i].name) {
                        propCount++;
                        if (game->properties[p].numBuildings == 5) hotelCount++;
                    }
                }
                printf("Properties : %d\n\n", propCount);
                printf("Hotels : %d\n\n", hotelCount);

                if (game->players[i].loan.amount > 0) {
                    printf("Outstanding Loan : LKR %d\n", game->players[i].loan.amount);
                } else {
                    printf("Outstanding Loan : None\n");
                }

                if (i < 3) {
                    printf("--------------------------------------------\n");
                }
            }
            printf("--------------------------------------------\n\n");

            //Per Round Changes
            checkInsuranceExpiry(game);
            incrementPropertyAges(game);
            decayBuildingConditions(game);
            checkStructuralDamage(game);
            autoRepairDamagedProperties(game);

            if (currentRound > 0 && currentRound % 10 == 0) {
                triggerDisaster(game, -1);
                applyInflation(game);
                updatePropertyMarket(game, currentRound);
            }

            if (currentRound > 0 && currentRound % 15 == 0) {
               updateRegionalDevelopment(game, currentRound);
               applyEconomicEvent(game);
            }

            if (currentRound > 0 && currentRound % 20 == 0) {
                applyGovernmentRegulation(game);
            }

            printMarketConditions(game);

            lastRound = currentRound;

            printf("============================================\n\n");
            printf("Round %d:\n", currentRound + 1);
        }

        if (currentRound >= maxRounds) {
            declareWinnerByNetWorth(game);
            break;
        }        
        
        for (int i = 0; i < 4; i++) {
            if (game->players[i].isBankrupt) {
                continue;
            }

            resolveJail(game, i);

            int diceVal = rollDice();
            printf("%s rolled %d\n", playerName(game->players[i].name), diceVal);

            movePlayer(i, diceVal, game);
            resolveLanding(game, i, diceVal);

            for (int p = 0; p < 22; p++) { //Construction can be done without being on that square
                if (game->properties[p].owner == game->players[i].name) {
                    int boardIdx = game->properties[p].boardIndex;
                    PropertyGroup group = game->properties[p].group;

                    if (!hasMonopoly(game, game->players[i].name, group)) {
                        continue;
                    }

                    if (game->properties[p].numBuildings == 4) {
                        buildHotel(game, i, boardIdx);
                    } else if (game->properties[p].numBuildings < 4) {
                        buildHouse(game, i, boardIdx);
                    }
                }

                if (game->properties[p].owner == game->players[i].name && game->properties[p].numBuildings > 0) {
                    if (game->properties[p].conditionRating < 90) {
                        maintainBuilding(game, i, game->properties[p].boardIndex);
                    }
                }

                if (game->properties[p].isDamaged) {
                    repairDamage(game, i, game->properties[p].boardIndex);
                }
            }

            printf("\n");
        }
    }
}
