//
// Created by Eric Thomas on 10/3/23.
//

#pragma once
#include <map>
#include <set>

using std::map;
using std::pair;

// This class stores current values for
//      - each firm's capital       at the end of the most recent micro time step
//      - the revenue               in the most recent micro time step for each firm-market combination
//      - the fixed cost            in the most recent micro time step for each firm-market combination
//      - the variable cost         in the most recent micro time step for each firm-market combination
//      - the entry cost            in the most recent micro time step for each firm-market combination
//      - the quantity produced     in the most recent micro time step for each firm-market combination

class DataCache {
public:
    // Default constructor
    DataCache();

    map<int,double> mapFirmCapital;
    map<pair<int, int>, double> mapFirmMarketComboToRevenue;
    map<pair<int, int>, double> mapFirmMarketComboToFixedCost;
    map<pair<int, int>, double> mapFirmMarketComboToVarCost;
    map<pair<int, int>, double> mapFirmMarketComboToEntryCost;
    map<pair<int, int>, double> mapFirmMarketComboToQtyProduced;
};
