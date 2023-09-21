//
// Created by Eric Thomas on 9/5/23.
//

#include "Market.h"

Market::Market(double dbFixedCostAsPercentageOfEntryCost, double dbVarCost,
               double dbDemandIntercept, double dbDemandSlope, const vector<int> &vecCapabilities) :
               dbFixedCostAsPercentageOfEntryCost(dbFixedCostAsPercentageOfEntryCost),
               dbVarCost(dbVarCost),
               dbDemandIntercept(dbDemandIntercept),
               dbDemandSlope(dbDemandSlope),
               vecCapabilities(vecCapabilities) {}

// Getters
const vector<int> &Market::get_vec_capabilities() const {return vecCapabilities;};
const int         &Market::get_market_id()        const {return iMarketID;};
