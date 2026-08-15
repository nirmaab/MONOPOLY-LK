#ifndef TYPES_H
#define TYPES_H

typedef enum {
    Bank, AGGRESSIVE_INVESTOR, CONSERVATIVE_BANKER, RISK_TAKER, OPPORTUNISTIC_TRADER
} PlayerName;

typedef struct {
    int amount;
    int interestRate;
    int roundsRemaining;
    int collateral[28]; //28 = 22properties + 2utilities + 4railways
    int collateralCount;
} Loan;

typedef struct {
    PlayerName name;
    int cash;
    int position; // board index(0-39)
    int inJail; //0=F or 1=F
    int jailTurns; //how many turns left in jail
    Loan loan;
    int lapsCompleted; //how many times travelled from 0 to 39

    //National Event Cards
    int activeCardIds[5];
    int activeCardExpiry[5];
    int activeCardCount;

    int hasExperiencedLoss; // set to 1 once a disaster damages an uninsured property they own
    
    int isBankrupt; 
} Player;

typedef enum {
    START, PROPERTY, EVENT, TAX, RAILWAY, UTILITY, INSURANCE, BANK, SPECIAL
} SquareType ;

typedef enum {
    BROWN, LIGHT_BLUE, PINK, ORANGE, RED, YELLOW, GREEN, DARK_BLUE
} PropertyGroup;

typedef struct{
    int index;
    SquareType type;
    char name[50];
} Square;

typedef enum {
    NONE, BASIC, COMPREHENSIVE, BUSINESS_INTERRUPTION
} InsurancePolicy;

typedef struct {
    int boardIndex; //matches the Square structs index in board arr
    PropertyGroup group; //ex brown, red etc..
    int purchasePrice;
    int mortgageValue;
    int baseRent;
    int houseCost;
    int hotelCost;
    PlayerName owner; //Initially owner is Bank
    int isMortgaged; //true=1, false=0
    InsurancePolicy insurancePolicy;
    int numBuildings; // 0=none, 1-4=houses, 5=hotel
    int propertyAge; // In rounds
    
    //For buidlings-->
    int conditionRating;
    int roundsSinceMaintenance;
    int isDamaged;

    int disasterDamaged;

    //National Event Cards-Political rally
    int closedUntilRound; // 0 = not closed, else the round it reopens

    int insuranceExpiryRound;
} PropertySquare;

typedef struct {
    int boardIndex;
    int purchasePrice;
    int mortgageValue;
    PlayerName owner;
    int isMortgaged;
} RailwaySquare;

typedef struct {
    int boardIndex;
    int purchasePrice;
    int mortgageValue;
    PlayerName owner;
    int isMortgaged;
} UtilitySquare;

typedef struct {
    int currentInterestRate;
    int incomeTaxRate;
    int communityFundTaxRate;

    //govregulations
    int railwayRentMultiplier;
    int utilityRentMultiplier;
    int insurancePremiumMultiplier;
    int antiSpeculationActive; // 0 or 1, permanent once triggered

    PropertyGroup marketBoomGroup;
    int marketBoomExpiryRound;
    PropertyGroup marketDeclineGroup;
    int marketDeclineExpiryRound;

    int regionalAffectedIndices[3];
    int regionalAffectedCount;
    int regionalPercent;
    int regionalIsRent;
    int regionalExpiryRound;

    //For National Event Cards-Property Revaluation
    PropertyGroup revaluedGroup;
    int revaluationExpiryRound;

    // Used for player behaviours
    int lastInflationRound;

    int lastInflationRate;
} EconomicState;

typedef struct {
    Square board[40];
    RailwaySquare railways[4];
    UtilitySquare utilities[2];
    PropertySquare properties[22];
    Player players[4];
    int lastAffectedRound[8]; //Round number each group(in PropertyGroup) was last boomed/declined
    int currentRound;
    EconomicState economy;
    int nationaleventscard;
} GameState;

typedef struct {
    int indices[3];
    int count;
    int percent;
    int isRent; // 1 = affects rent, 0 = affects value
} RegionalCard;

#endif
