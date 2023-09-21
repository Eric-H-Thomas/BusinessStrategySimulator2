//
// Created by Eric Thomas on 9/5/23.
//

#pragma once
#include <vector>

using std::vector;

class Market {
public:
    Market(double dbFixedCostAsPercentageOfEntryCost, double dbVarCost, double dbDemandIntercept, double dbDemandSlope,
           const vector<int> &vecCapabilities);
    const vector<int> &get_vec_capabilities() const;
    const int&         get_market_id() const;

private:
    int    iMarketID; // TODO: need to make sure this new member variable is fully implemented
    double dbFixedCostAsPercentageOfEntryCost;
    double dbVarCost;
    double dbDemandIntercept;
    double dbDemandSlope;
    vector<int> vecCapabilities;
};
