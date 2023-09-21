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