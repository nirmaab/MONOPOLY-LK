#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "functions.h"

//section 2.5

void applyEconomicEvent(GameState *game) {
    int pick = rand() % 8;

    printf("\nEconomic Event\n");

    switch (pick) {
        case 0: // Tourism Boom
            for (int i = 0; i < 22; i++) {
                if (game->properties[i].numBuildings == 5) {
                    game->properties[i].baseRent += game->properties[i].baseRent; // double
                }
            }
            int coastalIndices0[3] = {26, 27, 29};
            for (int i = 0; i < 3; i++) {
                PropertySquare *p = findProperty(game, coastalIndices0[i]);
                p->purchasePrice += p->purchasePrice * 15 / 100;
            }
            printf("Tourism Boom\n\n");
            printf("Hotels receive double rent. Southern coastal properties increase by 15%%.\n");
            break;

        case 1: // Fuel Crisis
            game->economy.railwayRentMultiplier = game->economy.railwayRentMultiplier * 2;
            for (int i = 0; i < 22; i++) {
                game->properties[i].houseCost += game->properties[i].houseCost * 20 / 100;
                game->properties[i].hotelCost += game->properties[i].hotelCost * 20 / 100;
            }
            printf("Fuel Crisis\n\n");
            printf("Railway rent doubles. Property development costs increase 20%%.\n");
            break;

        case 2: // Heavy Monsoon
        {
            int coastalIndices2[3] = {26, 27, 29};
            for (int i = 0; i < 3; i++) {
                PropertySquare *p = findProperty(game, coastalIndices2[i]);
                p->purchasePrice -= p->purchasePrice * 10 / 100;
            }
            game->economy.insurancePremiumMultiplier = game->economy.insurancePremiumMultiplier * 110 / 100;
            printf("Heavy Monsoon\n\n");
            printf("Insurance premiums increase. Coastal properties lose 10%% value.\n");
            break;
        }

        case 3: // Economic Recession
            for (int i = 0; i < 22; i++) {
                game->properties[i].purchasePrice -= game->properties[i].purchasePrice * 15 / 100;
                game->properties[i].baseRent -= game->properties[i].baseRent * 10 / 100;
            }
            game->economy.currentInterestRate += game->economy.currentInterestRate * 15 / 100;
            printf("Economic Recession\n\n");
            printf("Property values decrease 15%%. Rent decreases 10%%. Loan interest increases 15%%.\n");
            break;

        case 4: // Stock Market Boom
            for (int i = 0; i < 22; i++) {
                game->properties[i].purchasePrice += game->properties[i].purchasePrice * 10 / 100;
            }
            game->economy.currentInterestRate -= game->economy.currentInterestRate * 10 / 100;
            printf("Stock Market Boom\n\n");
            printf("Property values increase 10%%. Loan interest decreases 10%%.\n");
            break;

        case 5: // Government Housing Programme
            for (int i = 0; i < 22; i++) {
                game->properties[i].houseCost -= game->properties[i].houseCost * 25 / 100;
            }
            printf("Government Housing Programme\n\n");
            printf("House construction costs reduce 25%%.\n");
            break;

        case 6: // Foreign Investment
            for (int i = 0; i < 22; i++) {
                if (game->properties[i].numBuildings == 5) {
                    game->properties[i].purchasePrice += game->properties[i].purchasePrice * 20 / 100;
                }
            }
            printf("Foreign Investment\n\n");
            printf("Commercial property values increase 20%%.\n");
            break;

        case 7: // Political Unrest
            for (int i = 0; i < 22; i++) {
                if (game->properties[i].numBuildings == 5) {
                    game->properties[i].baseRent -= game->properties[i].baseRent * 50 / 100;
                }
            }
            printf("Political Unrest\n\n");
            printf("Hotel occupancy decreases and the hotel rent drops by 50%%.\n");
            break;

        default:
            break;
    }
}