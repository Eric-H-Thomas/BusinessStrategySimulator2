//
// Created by Eric Thomas on 9/7/23.
//

#include "SimulationHistory.h"

SimulationHistory::SimulationHistory(const map<int, int> &mapAgentToFirm,
                                     const map<int, double> &mapFirmStartingCapital,
                                     const map<int, double> &mapMarketMaximumEntryCost):
                                     mapAgentToFirm(mapAgentToFirm),
                                     mapFirmStartingCapital(mapFirmStartingCapital),
                                     mapMarketMaximumEntryCost(mapMarketMaximumEntryCost) {};

void SimulationHistory::record_market_presence_change(int iMicroTimeStep, bool bPresent, int iFirmID, int iMarketID) {
    MarketPresenceChange presenceChange;
    presenceChange.iMicroTimeStep = iMicroTimeStep;
    presenceChange.bPresent = bPresent;
    presenceChange.iFirmID = iFirmID;
    presenceChange.iMarketID = iMarketID;
    vecMarketPresenceChanges.push_back(presenceChange);
}

void SimulationHistory::record_capital_change(int iMicroTimeStep, int iFirmID, double dbNewCapitalQty) {
    CapitalChange capitalChange;
    capitalChange.iMicroTimeStep = iMicroTimeStep;
    capitalChange.iFirmID = iFirmID;
    capitalChange.dbNewCapitalQty = dbNewCapitalQty;
    vecCapitalChanges.push_back(capitalChange);
}

void SimulationHistory::record_revenue_change(int iMicroTimeStep, double dbNewRevenueAmount, int iFirmID, int iMarketID) {
    RevenueChange revenueChange;
    revenueChange.iMicroTimeStep = iMicroTimeStep;
    revenueChange.dbNewRevenueAmount = dbNewRevenueAmount;
    revenueChange.iFirmID = iFirmID;
    revenueChange.iMarketID = iMarketID;
    vecRevenueChanges.push_back(revenueChange);
}

void SimulationHistory::record_fixed_cost_change(int iMicroTimeStep, double dbNewFixedCost, int iFirmID, int iMarketID){
    FixedCostChange fixedCostChange;
    fixedCostChange.iMicroTimeStep = iMicroTimeStep;
    fixedCostChange.dbNewFixedCost = dbNewFixedCost;
    fixedCostChange.iFirmID = iFirmID;
    fixedCostChange.iMarketID = iMarketID;
    vecFixedCostChanges.push_back(fixedCostChange);
}

void SimulationHistory::record_entry_cost_change(int iMicroTimeStep, double dbNewEntryCost, int iFirmID, int iMarketID){
    EntryCostChange entryCostChange;
    entryCostChange.iMicroTimeStep = iMicroTimeStep;
    entryCostChange.dbNewEntryCost = dbNewEntryCost;
    entryCostChange.iFirmID = iFirmID;
    entryCostChange.iMarketID = iMarketID;
    vecEntryCostChanges.push_back(entryCostChange);
}

void SimulationHistory::record_production_quantity_change(int iMicroTimeStep, double dbNewProductionQty, int iFirmID, int iMarketID) {
    ProductionQuantityChange productionQuantityChange;
    productionQuantityChange.iMicroTimeStep = iMicroTimeStep;
    productionQuantityChange.dbNewProductionQty = dbNewProductionQty;
    productionQuantityChange.iFirmID = iFirmID;
    productionQuantityChange.iMarketID = iMarketID;
    vecProductionQtyChanges.push_back(productionQuantityChange);
}