#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "functions.h"

void applyGovernmentRegulation(GameState *game) {
    int pick = rand() % 8;

    printf("\nGovernment Regulation\n");

    switch (pick) {
        case 0:
            game->economy.incomeTaxRate = game->economy.incomeTaxRate * 150 / 100;
            printf("Increase Property Tax Introduced.\n\n");
            printf("Income Tax increased to %d%%.\n", game->economy.incomeTaxRate);
            break;
        case 1:
            game->economy.currentInterestRate -= 2;
            if (game->economy.currentInterestRate < 0) {
                game->economy.currentInterestRate = 0;
            }
            printf("Reduce Loan Interest Introduced.\n\n");
            printf("Interest decreased to %d%%.\n", game->economy.currentInterestRate);
            break;
        case 2:
            for (int i = 0; i < 22; i++) {
                game->properties[i].houseCost = game->properties[i].houseCost * 70 / 100;
            }
            printf("Housing Subsidy Introduced.\n\n");
            printf("Construction costs reduced by 30%%.\n");
            break;
        case 3: {
            for (int p = 0; p < 4; p++) {
                int tax = 0;
                for (int i = 0; i < 22; i++) {
                    if (game->properties[i].owner == game->players[p].name && game->properties[i].numBuildings == 5) {
                        tax += currentPropertyValue(game, &game->properties[i]) * 25 / 100;
                    }
                }
                attemptPayment(game, p, tax);
            }
            printf("Luxury Property Tax Introduced.\n\n");
            printf("Hotels incur an annual maintenance tax of 25%% of property value.\n");
            break;
        }
        case 4:
            game->economy.railwayRentMultiplier = game->economy.railwayRentMultiplier * 125 / 100;
            printf("Railway Modernization Introduced.\n\n");
            printf("Railway rents increased 25%%.\n");
            break;
        case 5:
            game->economy.utilityRentMultiplier = game->economy.utilityRentMultiplier * 120 / 100;
            printf("Electricity Tariff Revision Introduced.\n\n");
            printf("Utility rents increased 20%%.\n");
            break;
        case 6:
            game->economy.insurancePremiumMultiplier = game->economy.insurancePremiumMultiplier * 85 / 100;
            printf("Insurance Regulation Introduced.\n\n");
            printf("Premiums decreased 15%%.\n");
            break;
        case 7:
            game->economy.antiSpeculationActive = 1;
            printf("Anti-Speculation Act Introduced.\n\n");
            printf("Players may own at most three undeveloped properties.\n");
            break;
        default:
            break;
    }
}
