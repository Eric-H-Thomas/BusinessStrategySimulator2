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