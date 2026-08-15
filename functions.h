#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "types.h"

// board.c
void initBoard(GameState *game);

// players.c
void initPlayers(GameState *game);
const char* playerName(PlayerName p);
void movePlayer(int playerIndex, int diceVal, GameState *game);
int rollDice();
void resolveJail(GameState *game, int playerIndex);

// game.c
void determineTurnOrder(GameState *game);
void resolveLanding(GameState *game, int playerIndex, int diceVal);
void runGame(GameState *game);

// finance.c
PropertySquare* findProperty(GameState *game, int boardIndex);
void purchaseProperty(GameState *game, int playerIndex, int boardIndex);
void propertyRent(GameState *game, int playerIndex, int boardIndex);
RailwaySquare* findRailway(GameState *game, int boardIndex);
void purchaseRailway(GameState *game, int playerIndex, int boardIndex);
void railwayRent(GameState *game, int playerIndex, int boardIndex);
UtilitySquare* findUtility(GameState *game, int boardIndex);
void purchaseUtility(GameState *game, int playerIndex, int boardIndex);
void utilityRent(GameState *game, int playerIndex, int boardIndex, int diceVal);
int totalCollateralValue(GameState *game, PlayerName player);
int maxLoanAmount(GameState *game, PlayerName player);
void takeLoan(GameState *game, int playerIndex, int loanAmount);
void repayLoan(GameState *game, int playerIndex);
void foreclose(GameState *game, int playerIndex);
void mortgageProperty(GameState *game, int playerIndex, int boardIndex);
void mortgageRailway(GameState *game, int playerIndex, int boardIndex);
void mortgageUtility(GameState *game, int playerIndex, int boardIndex);
void payAssetTax(GameState *game, int playerIndex, int percent);

// buildings.c
int hasMonopoly(GameState *game, PlayerName player, PropertyGroup group);
int canBuildEvenly(GameState *game, PropertyGroup group, int boardIndex);
void buildHouse(GameState *game, int playerIndex, int boardIndex);
void buildHotel(GameState *game, int playerIndex, int boardIndex);
int calculateDepreciation(int propertyAge);
int currentPropertyValue(GameState *game, PropertySquare *prop);
void renovateProperty(GameState *game, int playerIndex, int boardIndex);
void incrementPropertyAges(GameState *game);
void decayBuildingConditions(GameState *game);
void maintainBuilding(GameState *game, int playerIndex, int boardIndex);
void checkStructuralDamage(GameState *game);
void repairDamage(GameState *game, int playerIndex, int boardIndex);

// insurance.c
void purchaseInsurance(GameState *game, int playerIndex, int boardIndex, InsurancePolicy policyType);
void checkInsuranceExpiry(GameState *game);
void triggerDisaster(GameState *game, int playerIndex);
void autoRepairDamagedProperties(GameState *game);

// inflation.c
void applyInflation(GameState *game);

// dyanmicpropmarket.c
const char* groupName(PropertyGroup group);
PropertyGroup pickEligibleGroup(GameState *game, int currentRound, PropertyGroup exclude);
void updatePropertyMarket(GameState *game, int currentRound);
void printMarketConditions(GameState *game);

//regionaldev.c
void updateRegionalDevelopment(GameState *game, int currentRound);

//govregulations.c
void applyGovernmentRegulation(GameState *game);

//nationaleventcards.c
int hasActiveCard(Player *player, int cardId, int currentRound);
void applyNationalEventCard(GameState *game, int playerIndex);

//behaviours.c
int decideWhetherToBuyProperty(GameState *game, int playerIndex, int boardIndex);
int decideWhetherToBuyRailway(GameState *game, int playerIndex, int boardIndex);
int decideWhetherToBuyUtility(GameState *game, int playerIndex, int boardIndex);
int decideAuctionBid(GameState *game, int playerIndex, int basePrice, int currentBid);
int decideWhetherToBuild(GameState *game, int playerIndex, int boardIndex);
int decideInsurancePolicy(GameState *game, int playerIndex, int boardIndex);
int decideWhetherToTakeLoan(GameState *game, int playerIndex);
int decideWhetherToRepay(GameState *game, int playerIndex);

//auctions.c
void startAuction(GameState *game, SquareType type, int boardIndex);

//gameend.c
int calculateNetWorth(GameState *game, int playerIndex);
void declareBankruptcy(GameState *game, int playerIndex);
void attemptPayment(GameState *game, int playerIndex, int amount);
int checkGameEnd(GameState *game);
void declareWinnerByNetWorth(GameState *game);

//economicevents.c
void applyEconomicEvent(GameState *game);

#endif