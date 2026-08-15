#include "types.h"

void initBoard(GameState *game) {
    Square boardData[40] = {
        {0,  START,     "GO"},
        {1,  PROPERTY,  "Pettah"},
        {2,  EVENT,     "Community Development Fund"},
        {3,  PROPERTY,  "Maradana"},
        {4,  TAX,       "Income Tax"},
        {5,  RAILWAY,   "Colombo Fort Railway Station"},
        {6,  PROPERTY,  "Bambalapitiya"},
        {7,  EVENT,     "National Event Card"},
        {8,  PROPERTY,  "Wellawatte"},
        {9,  PROPERTY,  "Mount Lavinia"},
        {10, SPECIAL,   "Jail / Just Visiting"},
        {11, PROPERTY,  "Nugegoda"},
        {12, UTILITY,   "Ceylon Electricity Board"},
        {13, PROPERTY,  "Maharagama"},
        {14, PROPERTY,  "Kottawa"},
        {15, RAILWAY,   "Kandy Railway Station"},
        {16, PROPERTY,  "Negombo"},
        {17, INSURANCE, "Sri Lanka Insurance"},
        {18, PROPERTY,  "Katunayake"},
        {19, PROPERTY,  "Ja-Ela"},
        {20, SPECIAL,   "Free Parking"},
        {21, PROPERTY,  "Kandy City"},
        {22, EVENT,     "National Event Card"},
        {23, PROPERTY,  "Peradeniya"},
        {24, PROPERTY,  "Katugastota"},
        {25, RAILWAY,   "Galle Railway Station"},
        {26, PROPERTY,  "Galle Fort"},
        {27, PROPERTY,  "Unawatuna"},
        {28, UTILITY,   "National Water Supply and Drainage Board"},
        {29, PROPERTY,  "Hikkaduwa"},
        {30, SPECIAL,   "Go To Jail"},
        {31, PROPERTY,  "Jaffna Town"},
        {32, PROPERTY,  "Nallur"},
        {33, INSURANCE, "Ceylinco Insurance"},
        {34, PROPERTY,  "Trincomalee"},
        {35, RAILWAY,   "Jaffna Railway Station"},
        {36, EVENT,     "National Event Card"},
        {37, PROPERTY,  "Nuwara Eliya"},
        {38, BANK,      "Bank of Ceylon"},
        {39, PROPERTY,  "Galle Face"},
    };

    for (int i = 0; i < 40; i++) {
        game->board[i] = boardData[i];
    }

    PropertySquare propertyData[22] = {
        {1,  BROWN,      1500, 750,  100, 500,  2000, Bank, 0, NONE, 0, 0, 100, 0, 0, 0, 0, 0},
        {3,  BROWN,      1800, 750,  120, 500,  2000, Bank, 0, NONE, 0, 0, 100, 0, 0, 0, 0, 0},
        {6,  LIGHT_BLUE, 2500, 1250, 180, 750,  3000, Bank, 0, NONE, 0, 0, 100, 0, 0, 0, 0, 0},
        {8,  LIGHT_BLUE, 2700, 1250, 200, 750,  3000, Bank, 0, NONE, 0, 0, 100, 0, 0, 0, 0, 0},
        {9,  LIGHT_BLUE, 3000, 1250, 220, 750,  3000, Bank, 0, NONE, 0, 0, 100, 0, 0, 0, 0, 0},
        {11, PINK,       3500, 1750, 260, 1000, 4000, Bank, 0, NONE, 0, 0, 100, 0, 0, 0, 0, 0},
        {13, PINK,       3800, 1750, 280, 1000, 4000, Bank, 0, NONE, 0, 0, 100, 0, 0, 0, 0, 0},
        {14, PINK,       4000, 1750, 300, 1000, 4000, Bank, 0, NONE, 0, 0, 100, 0, 0, 0, 0, 0},
        {16, ORANGE,     4500, 2250, 350, 1250, 5000, Bank, 0, NONE, 0, 0, 100, 0, 0, 0, 0, 0},
        {18, ORANGE,     4700, 2250, 370, 1250, 5000, Bank, 0, NONE, 0, 0, 100, 0, 0, 0, 0, 0},
        {19, ORANGE,     5000, 2250, 400, 1250, 5000, Bank, 0, NONE, 0, 0, 100, 0, 0, 0, 0, 0},
        {21, RED,        5500, 2750, 450, 1500, 6000, Bank, 0, NONE, 0, 0, 100, 0, 0, 0, 0, 0},
        {23, RED,        5800, 2750, 480, 1500, 6000, Bank, 0, NONE, 0, 0, 100, 0, 0, 0, 0, 0},
        {24, RED,        6000, 2750, 500, 1500, 6000, Bank, 0, NONE, 0, 0, 100, 0, 0, 0, 0, 0},
        {26, YELLOW,     6500, 3250, 600, 2000, 8000, Bank, 0, NONE, 0, 0, 100, 0, 0, 0, 0, 0},
        {27, YELLOW,     6800, 3250, 620, 2000, 8000, Bank, 0, NONE, 0, 0, 100, 0, 0, 0, 0, 0},
        {29, YELLOW,     7000, 3250, 650, 2000, 8000, Bank, 0, NONE, 0, 0, 100, 0, 0, 0, 0, 0},
        {31, GREEN,      8000, 4000, 750, 2500, 10000, Bank, 0, NONE, 0, 0, 100, 0, 0, 0, 0, 0},
        {32, GREEN,      8300, 4000, 780, 2500, 10000, Bank, 0, NONE, 0, 0, 100, 0, 0, 0, 0, 0},
        {34, GREEN,      8500, 4000, 800, 2500, 10000, Bank, 0, NONE, 0, 0, 100, 0, 0, 0, 0, 0},
        {37, DARK_BLUE, 10000, 5000, 1000, 3000, 12000, Bank, 0, NONE, 0, 0, 100, 0, 0, 0, 0, 0},
        {39, DARK_BLUE, 12000, 5000, 1200, 3000, 12000, Bank, 0, NONE, 0, 0, 100, 0, 0, 0, 0, 0},
    };
    for (int i = 0; i < 22; i++) {
        game->properties[i] = propertyData[i];
    }

    RailwaySquare railwayData[4] = {
        {5,  1500, 750, Bank, 0},
        {15, 1500, 750, Bank, 0},
        {25, 1500, 750, Bank, 0},
        {35, 1500, 750, Bank, 0},
    };
    for (int i = 0; i < 4; i++) {
        game->railways[i] = railwayData[i];
    }

    UtilitySquare utilityData[2] = {
        {12, 1500, 750, Bank, 0},
        {28, 1500, 750, Bank, 0},
    };
    for (int i = 0; i < 2; i++) {
        game->utilities[i] = utilityData[i];
    }

    game->economy.currentInterestRate = 8; // Stable Economy

    for (int i = 0; i < 8; i++) {
        game->lastAffectedRound[i] = -30;
    }
    game->economy.currentInterestRate = 8;

    game->economy.incomeTaxRate = 15;
    game->economy.communityFundTaxRate = 10;

    game->economy.insurancePremiumMultiplier = 100;
    game->economy.railwayRentMultiplier = 100;
    game->economy.utilityRentMultiplier = 100;
    game->economy.antiSpeculationActive = 0;

    game->economy.lastInflationRound = -1;
    game->economy.lastInflationRate = 0;

    game->nationaleventscard = 0;

    game->economy.marketBoomExpiryRound = 0;
    game->economy.marketDeclineExpiryRound = 0;
    game->economy.regionalExpiryRound = 0;
}