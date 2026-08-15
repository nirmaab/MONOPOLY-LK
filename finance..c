#include <stdio.h>

#include "types.h"
#include "functions.h"

// Propery Finances (Indetify sq, purchase or pay rent)
PropertySquare* findProperty(GameState *game, int boardIndex) {
    for (int i = 0; i < 22; i++) {
        if(game->properties[i].boardIndex == boardIndex) {
            return &game->properties[i];
        }
    }
    return 0;
}

void purchaseProperty(GameState *game, int playerIndex, int boardIndex) {
    PropertySquare *property = findProperty(game, boardIndex);

    if (property == 0) {
        return;
    }

    int price = property->purchasePrice;
    if (property->group == game->economy.marketBoomGroup && 
        game->currentRound < game->economy.marketBoomExpiryRound) 
    {
        price = price * 115 / 100;
    } else if (property->group == game->economy.marketDeclineGroup && 
        game->currentRound < game->economy.marketDeclineExpiryRound) 
    {
        price = price * 85 / 100;
    }


    if (game->economy.antiSpeculationActive) { //Gov Regulation
        int undevelopedCount = 0;
        for (int i = 0; i < 22; i++) {
            if (game->properties[i].owner == game->players[playerIndex].name && game->properties[i].numBuildings == 0) {
                undevelopedCount++;
            }
        }
        if (undevelopedCount >= 3) {
            printf("%s cannot purchase more undeveloped properties (Anti-Speculation Act).\n",
                playerName(game->players[playerIndex].name));
            return;
        }
    }

    if (game->players[playerIndex].cash < price) {
        printf("%s cannot afford %s.\n", 
               playerName(game->players[playerIndex].name), 
               game->board[boardIndex].name);
        return;
    }

    game->players[playerIndex].cash -= price;
    property->owner = game->players[playerIndex].name;

    printf("%s purchased %s for LKR %d.\n",
           playerName(game->players[playerIndex].name),
           game->board[boardIndex].name,
           price);

    printf("Remaining Balance : LKR %d.\n", game->players[playerIndex].cash);
}

void propertyRent(GameState *game, int playerIndex, int boardIndex) {
    PropertySquare *property = findProperty(game, boardIndex);

    if (property->isMortgaged) {
        printf("%s is mortgaged. No rent collected.\n", game->board[boardIndex].name);
        return;
    }

    if (property->disasterDamaged) {
        printf("%s is damaged from a disaster. No rent collected.\n", game->board[boardIndex].name);
        return;
    }

    int ownerIndex = -1;
    for (int i = 0; i < 4; i++) {
        if (game->players[i].name == property->owner) {
            ownerIndex = i;
            break;
        }
    }

    int rent = property->baseRent;

    if (property->isDamaged) {
        rent = rent * 75 / 100;
    }

    if (game->currentRound < property->closedUntilRound) {
        printf("%s is closed. No rent collected.\n", game->board[boardIndex].name);
        return;
    }

    if (property->group == game->economy.marketBoomGroup && 
        game->currentRound < game->economy.marketBoomExpiryRound) {
        rent = rent * 125 / 100;
    } else if (property->group == game->economy.marketDeclineGroup && 
        game->currentRound < game->economy.marketDeclineExpiryRound) {
        rent = rent * 80 / 100;
    }

    for (int i = 0; i < game->economy.regionalAffectedCount; i++) {
        if (game->economy.regionalAffectedIndices[i] == property->boardIndex && 
            game->currentRound < game->economy.regionalExpiryRound && 
            game->economy.regionalIsRent) 
        {
            rent = rent + (rent * game->economy.regionalPercent / 100);
            break;
        }
    }

    if (property->numBuildings > 0) {
        int condition = property->conditionRating;

        if (condition >= 90) {
            rent = property->baseRent;
        } else if (condition >= 75) {
            rent = property->baseRent * 90 / 100;
        } else if (condition >= 50) {
            rent = property->baseRent * 75 / 100;
        } else if (condition >= 25) {
            rent = property->baseRent * 50 / 100;
        } else {
            printf("%s is closed for maintenance. No rent collected.\n", game->board[boardIndex].name);
            return;
        }
    }

    if (property->numBuildings == 5 && ownerIndex != -1 && 
        hasActiveCard(&game->players[ownerIndex], 0, game->currentRound)) {
        rent = rent * 2;
    } //Tourism Hype-national event cards

    if (property->numBuildings == 5 && ownerIndex != -1 && 
        hasActiveCard(&game->players[ownerIndex], 13, game->currentRound)) {
        rent = rent + (rent * 50 / 100);
    } //Festival Season-national event cards

    attemptPayment(game, playerIndex, rent);
    game->players[ownerIndex].cash += rent;

    printf("\nRent Paid : LKR %d.\n", rent);
    printf("Owner : %s.\n", playerName(game->players[ownerIndex].name));
}

// Railway Finance (Indetify sq, purchase or pay rent)
RailwaySquare* findRailway(GameState *game, int boardIndex) {
    for (int i = 0; i < 4; i++) {
        if(game->railways[i].boardIndex == boardIndex) {
            return &game->railways[i];
        }
    }
    return 0;
}

void purchaseRailway(GameState *game, int playerIndex, int boardIndex) {
    RailwaySquare *railway = findRailway(game, boardIndex);

    if (railway == 0) {
        return; // not a valid railway
    }

    int price = railway->purchasePrice;
    if (hasActiveCard(&game->players[playerIndex], 12, game->currentRound)) { 
        price = price * 120 / 100;
    } //Port Expansion-national event cards

    if (game->players[playerIndex].cash < price) {
        printf("%s cannot afford %s.\n", 
               playerName(game->players[playerIndex].name), 
               game->board[boardIndex].name);
        return;
    }

    game->players[playerIndex].cash -= price;
    railway->owner = game->players[playerIndex].name;

    printf("%s purchased %s for LKR %d.\n",
           playerName(game->players[playerIndex].name),
           game->board[boardIndex].name,
           price);

    printf("Remaining Balance : LKR %d.\n", game->players[playerIndex].cash);
}

void railwayRent(GameState *game, int playerIndex, int boardIndex) {
    RailwaySquare *railway = findRailway(game, boardIndex);

    if (railway->isMortgaged) {
        printf("%s is mortgaged. No rent collected.\n", game->board[boardIndex].name);
        return;
    }

    int ownerIndex = -1;
    for (int i = 0; i < 4; i++) {
        if (game->players[i].name == railway->owner) {
            ownerIndex = i;
            break;
        }
    }

    int stationsOwned = 0;
    for (int i = 0; i < 4; i++) {
        if (game->railways[i].owner == railway->owner) {
            stationsOwned++;
        }
    }

    int rent;
    switch (stationsOwned) {
        case 1: rent = 250; break;
        case 2: rent = 500; break;
        case 3: rent = 1000; break;
        case 4: rent = 2000; break;
        default: rent = 0; break;
    }

    rent = rent * game->economy.railwayRentMultiplier / 100;

    if (ownerIndex != -1 && hasActiveCard(&game->players[ownerIndex], 1, game->currentRound)) {
        rent = rent * 2;
    } //Fuel Shortage-national event cards

    attemptPayment(game, playerIndex, rent);
    game->players[ownerIndex].cash += rent;

    printf("\nRent Paid : LKR %d.\n", rent);
    printf("Owner : %s.\n", playerName(game->players[ownerIndex].name));
}

// Utility Finance (Indetify sq, purchase or pay rent)
UtilitySquare* findUtility(GameState *game, int boardIndex) {
    for (int i = 0; i < 2; i++) {
        if(game->utilities[i].boardIndex == boardIndex) {
            return &game->utilities[i];
        }
    }
    return 0;
}

void purchaseUtility(GameState *game, int playerIndex, int boardIndex) {
    UtilitySquare *utility = findUtility(game, boardIndex);

    if (utility == 0) {
        return; // not a valid utility
    }

    if (game->players[playerIndex].cash < utility->purchasePrice) {
        printf("%s cannot afford %s.\n", 
               playerName(game->players[playerIndex].name), 
               game->board[boardIndex].name);
        return;
    }

    game->players[playerIndex].cash -= utility->purchasePrice;
    utility->owner = game->players[playerIndex].name;

    printf("%s purchased %s for LKR %d.\n",
           playerName(game->players[playerIndex].name),
           game->board[boardIndex].name,
           utility->purchasePrice);

    printf("Remaining Balance : LKR %d.\n", game->players[playerIndex].cash);
}

void utilityRent(GameState *game, int playerIndex, int boardIndex, int diceVal) {
    UtilitySquare *utility = findUtility(game, boardIndex);

    if (utility->isMortgaged) {
        printf("%s is mortgaged. No rent collected.\n", game->board[boardIndex].name);
        return;
    }

    int ownerIndex = -1;
    for (int i = 0; i < 4; i++) {
        if (game->players[i].name == utility->owner) {
            ownerIndex = i;
            break;
        }
    }

    int utilitiesOwned = 0;
    for (int i = 0; i < 2; i++) {
        if (game->utilities[i].owner == utility->owner) {
            utilitiesOwned++;
        }
    }

    int multiplier = (utilitiesOwned == 2) ? 10 : 4;
    int rent = diceVal * multiplier;
    rent = rent * game->economy.utilityRentMultiplier / 100;

    if (ownerIndex != -1 && hasActiveCard(&game->players[ownerIndex], 10, game->currentRound)) {
        rent = rent / 2;
    } //Power Failure-national event cards

    attemptPayment(game, playerIndex, rent);
    game->players[ownerIndex].cash += rent;

    printf("\nRent Paid : LKR %d.\n", rent);
    printf("Owner : %s.\n", playerName(game->players[ownerIndex].name));
}

// Loan Finance
int totalCollateralValue(GameState *game, PlayerName player) {
    int total = 0;
    int groupMortgageValues[8] = {750, 1250, 1750, 2250, 2750, 3250, 4000, 5000};

    for (int i = 0; i < 22; i++) {
        if (game->properties[i].owner == player) {
            total += groupMortgageValues[game->properties[i].group];
        }
    }

    for (int i = 0; i < 4; i++) {
        if (game->railways[i].owner == player) {
            total += game->railways[i].mortgageValue;
        }
    }

    for (int i = 0; i < 2; i++) {
        if (game->utilities[i].owner == player) {
            total += game->utilities[i].mortgageValue;
        }
    }

    return total;
}

int maxLoanAmount(GameState *game, PlayerName player) {
    int total = totalCollateralValue(game, player);
    return (total * 75) / 100;
}

void takeLoan(GameState *game, int playerIndex, int loanAmount) {
    if (game->players[playerIndex].loan.amount > 0) {
        printf("%s already has an active loan.\n", 
               playerName(game->players[playerIndex].name));
        return;
    }

    PlayerName player = game->players[playerIndex].name;
    int maxLoan = maxLoanAmount(game, player);

    if (maxLoan == 0) {
        printf("%s has no eligible collateral for a loan.\n", playerName(player));
        return;
    }

    if (loanAmount > maxLoan) {
        printf("%s cannot borrow LKR %d (max allowed: LKR %d).\n",
               playerName(player), loanAmount, maxLoan);
        return;
    }

    int groupMortgageValues[8] = {750, 1250, 1750, 2250, 2750, 3250, 4000, 5000};
    int accumulated = 0;
    int collateralIndices[28];
    int collateralCount = 0;
    int neededValue = loanAmount * 100 / 75; // reversed the 75% max amount to find collateral needed

    for (int i = 0; i < 22 && accumulated < neededValue; i++) {
        if (game->properties[i].owner == player) {
            collateralIndices[collateralCount] = game->properties[i].boardIndex;
            collateralCount++;
            accumulated += groupMortgageValues[game->properties[i].group];
        }
    }
    for (int i = 0; i < 4 && accumulated < neededValue; i++) {
        if (game->railways[i].owner == player) {
            collateralIndices[collateralCount] = game->railways[i].boardIndex;
            collateralCount++;
            accumulated += game->railways[i].mortgageValue;
        }
    }
    for (int i = 0; i < 2 && accumulated < neededValue; i++) {
        if (game->utilities[i].owner == player) {
            collateralIndices[collateralCount] = game->utilities[i].boardIndex;
            collateralCount++;
            accumulated += game->utilities[i].mortgageValue;
        }
    }

    for (int i = 0; i < collateralCount; i++) {
        game->players[playerIndex].loan.collateral[i] = collateralIndices[i];
    }
    game->players[playerIndex].loan.collateralCount = collateralCount;

    game->players[playerIndex].loan.amount = loanAmount;

    int rate = game->economy.currentInterestRate;
    if (hasActiveCard(&game->players[playerIndex], 6, game->currentRound)) {
        rate -= 2;
    }
    if (hasActiveCard(&game->players[playerIndex], 7, game->currentRound)) {
        rate += 2;
    }
    game->players[playerIndex].loan.interestRate = rate;
    game->players[playerIndex].loan.roundsRemaining = 20;
    game->players[playerIndex].cash += loanAmount;

    printf("%s obtained a secured loan.\n\n", playerName(player));
    printf("Loan Amount : LKR %d.\n\n", loanAmount);
    printf("Collateral :\n");
    for (int i = 0; i < collateralCount; i++) {
        printf("%s\n", game->board[collateralIndices[i]].name);
    }
    printf("\nInterest Rate : %d%%\n", rate);
    printf("Duration : 20 Rounds\n");
}

void mortgageProperty(GameState *game, int playerIndex, int boardIndex) {
    PropertySquare *property = findProperty(game, boardIndex);

    if (property->owner != game->players[playerIndex].name) {
        printf("%s does not own %s.\n",
               playerName(game->players[playerIndex].name),
               game->board[boardIndex].name);
        return;
    }

    if (property->isMortgaged) {
        printf("%s is already mortgaged.\n", game->board[boardIndex].name);
        return;
    }

    property->isMortgaged = 1;
    game->players[playerIndex].cash += property->mortgageValue;

    printf("%s mortgaged %s for LKR %d.\n",
        playerName(game->players[playerIndex].name),
        game->board[boardIndex].name,
        property->mortgageValue);
}

void mortgageRailway(GameState *game, int playerIndex, int boardIndex) {
    RailwaySquare *railway = findRailway(game, boardIndex);

    if (railway->owner != game->players[playerIndex].name) {
        printf("%s does not own %s.\n",
               playerName(game->players[playerIndex].name),
               game->board[boardIndex].name);
        return;
    }

    if (railway->isMortgaged) {
        printf("%s is already mortgaged.\n", game->board[boardIndex].name);
        return;
    }

    railway->isMortgaged = 1;
    game->players[playerIndex].cash += railway->mortgageValue;

    printf("%s mortgaged %s for LKR %d.\n",
        playerName(game->players[playerIndex].name),
        game->board[boardIndex].name,
        railway->mortgageValue);
}

void mortgageUtility(GameState *game, int playerIndex, int boardIndex) {
    UtilitySquare *utility = findUtility(game, boardIndex);

    if (utility->owner != game->players[playerIndex].name) {
        printf("%s does not own %s.\n",
               playerName(game->players[playerIndex].name),
               game->board[boardIndex].name);
        return;
    }

    if (utility->isMortgaged) {
        printf("%s is already mortgaged.\n", game->board[boardIndex].name);
        return;
    }

    utility->isMortgaged = 1;
    game->players[playerIndex].cash += utility->mortgageValue;

    printf("%s mortgaged %s for LKR %d.\n",
        playerName(game->players[playerIndex].name),
        game->board[boardIndex].name,
        utility->mortgageValue);
}

void repayLoan(GameState *game, int playerIndex) {
    int owed = game->players[playerIndex].loan.amount;

    if (owed == 0) {
        printf("%s has no active loan.\n", playerName(game->players[playerIndex].name));
        return;
    }

    if (game->players[playerIndex].cash < owed) {
        printf("%s cannot fully repay LKR %d (insufficient cash).\n",
               playerName(game->players[playerIndex].name), owed);
        return;
    }

    game->players[playerIndex].cash -= owed;
    game->players[playerIndex].loan.amount = 0;
    game->players[playerIndex].loan.interestRate = 0;
    game->players[playerIndex].loan.roundsRemaining = 0;
    game->players[playerIndex].loan.collateralCount = 0;

    printf("%s repaid LKR %d.\n\n", playerName(game->players[playerIndex].name), owed);
    printf("Outstanding Balance :\nLKR 0.\n\n");
}

void foreclose(GameState *game, int playerIndex) {
    PlayerName player = game->players[playerIndex].name;

    printf("%s has defaulted.\n\n", playerName(player));
    printf("Collateral has been foreclosed.\n\n");

    for (int c = 0; c < game->players[playerIndex].loan.collateralCount; c++) {
        int boardIdx = game->players[playerIndex].loan.collateral[c];
        SquareType type = game->board[boardIdx].type;

        if (type == PROPERTY) {
            PropertySquare *p = findProperty(game, boardIdx);
            p->owner = Bank;
            p->isMortgaged = 0;
            p->numBuildings = 0;
            p->insurancePolicy = NONE;
        } else if (type == RAILWAY) {
            RailwaySquare *r = findRailway(game, boardIdx);
            r->owner = Bank;
            r->isMortgaged = 0;
        } else if (type == UTILITY) {
            UtilitySquare *u = findUtility(game, boardIdx);
            u->owner = Bank;
            u->isMortgaged = 0;
        }
    }

    game->players[playerIndex].loan.amount = 0;
    game->players[playerIndex].loan.interestRate = 0;
    game->players[playerIndex].loan.roundsRemaining = 0;
    game->players[playerIndex].loan.collateralCount = 0;

    printf("Outstanding debt cleared.\n\n");

    if (game->players[playerIndex].cash <= 0) {
        declareBankruptcy(game, playerIndex);
    }
}

void payAssetTax(GameState *game, int playerIndex, int percent) {
    int total = 0;
    for (int i = 0; i < 22; i++) {
        if (game->properties[i].owner == game->players[playerIndex].name) {
            total += currentPropertyValue(game, &game->properties[i]);
        }
    }

    int tax = total * percent / 100;
    attemptPayment(game, playerIndex, tax);

    printf("%s paid LKR %d tax (%d%% of property value).\n",
           playerName(game->players[playerIndex].name), tax, percent);
}