//
// Created by Eric Thomas on 9/7/23.
//

#pragma once

#include <vector>
#include <map>

using std::map;
using std::vector;

// To keep the history as lightweight as possible, we will record changes in the states of simulator objects
// rather than recording values for such objects at every time step. For example, rather than recording that firm A was
// present in market X for time steps 1, 2 and 3 and not present for steps 4, 5, and 6, we will just record an entrance
// at time step 1 and an exit at time step 4.

// Predeclare the structs so that we can keep the SimulationHistory info at the top of the file
struct CapitalChange; struct RevenueChange; struct FixedCostChange; struct VarCostChange;
struct EntryCostChange; struct QuantityProducedChange; struct MarketPresenceChange;

class SimulationHistory {
public:
    map<int,int>                    mapAgentToFirm;
    map<int,double>                 mapFirmStartingCapital;
    map<int,double>                 mapMarketMaximumEntryCost;
    vector<CapitalChange>           vecCapitalChanges;

    SimulationHistory(const map<int,int> &mapAgentToFirm, const map<int,double> &mapFirmStartingCapital,
                      const map<int,double> &mapMarketMaximumEntryCost);

    vector<RevenueChange>           vecRevenueChanges;
    vector<FixedCostChange>         vecFixedCostChanges;
    vector<VarCostChange>           vecVarCostChanges;
    vector<EntryCostChange>         vecEntryCostChanges;
    vector<QuantityProducedChange>  vecQtyProducedChanges;
    vector<MarketPresenceChange>    vecMarketPresenceChanges;
};

struct CapitalChange {
    int iMicroTimeStep;
    int iFirmID;
    double dbNewCapitalQty;
};

struct RevenueChange {
    int iMicroTimeStep;
    double dbNewRevenueAmount;
    int iFirmID;
    int iMarketID;
};

struct FixedCostChange {
    int iMicroTimeStep;
    double dbNewFixedCost;
    int iFirmID;
    int iMarketID;
};

struct VarCostChange {
    int iMicroTimeStep;
    double dbNewVarCost;
    int iFirmID;
    int iMarketID;
};

struct EntryCostChange {
    int iMicroTimeStep;
    double dbNewEntryCost;
    int iFirmID;
    int iMarketID;
};

struct QuantityProducedChange {
    int iMicroTimeStep;
    double dbNewQtyProduced;
    int iFirmID;
    int iMarketID;
};

struct MarketPresenceChange {
    int iMicroTimeStep;
    bool bPresent;
    int iFirmID;
    int iMarketID;
};