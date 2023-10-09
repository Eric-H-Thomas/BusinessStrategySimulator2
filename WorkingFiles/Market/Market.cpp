//
// Created by Eric Thomas on 9/5/23.
//

#include "Market.h"

// Default constructor
Market::Market() {}

// Parameterized constructor
Market::Market(double dbFixedCostAsPercentageOfEntryCost, double dbDemandIntercept, double dbDemandSlope,
               const vector<int> &vecCapabilities) :
               dbFixedCostAsPercentageOfEntryCost(dbFixedCostAsPercentageOfEntryCost),
               dbDemandIntercept(dbDemandIntercept),
               dbDemandSlope(dbDemandSlope),
               vecCapabilities(vecCapabilities) {}

// Getters
const vector<int> &Market::get_vec_capabilities() const {return vecCapabilities;};
const int         &Market::get_market_id()        const {return iMarketID;};

const double      &Market::getDbDemandIntercept() const {return dbDemandIntercept;}
const double      &Market::getDbDemandSlope()     const {return dbDemandSlope;}



// Comparison operator (necessary in order to put Markets in sets)
bool Market::operator<(const Market& other) const {
    return this->iMarketID < other.iMarketID;
}




