//
// Created by Eric Thomas on 8/31/23.
//

#include "Firm.h"
#include "../Utils/MiscUtils.h"

Firm::Firm() = default;

Firm::Firm(int iFirmID, double dbStartingCapital) {
    this->iFirmID = iFirmID;
    this->dbCapital = dbStartingCapital;
}

int Firm::getFirmID() const {
    return iFirmID;
}

double Firm::getDbCapital() const {
    return dbCapital;
}

bool Firm::is_in_market(Market market){
   if (this->setMarketIDs.find(market.get_market_id()) != setMarketIDs.end()){
       return true;
   }
   return false;
}

Market Firm::choose_market_with_highest_overlap(set<Market> setMarkets){
    double dbHighestOverlapValue = 0.0;
    set<Market> setMarketsWithHighestOverlap;

    for (Market market : setMarkets) {
        double dbOverlap = MiscUtils::get_percentage_overlap(market.get_vec_capabilities(), this->vecCapabilities);
        if (dbOverlap >= dbHighestOverlapValue) {
            dbHighestOverlapValue = dbOverlap;
            setMarketsWithHighestOverlap.insert(market);
        }
    }

    if (setMarketsWithHighestOverlap.empty()) {
        std::cerr << "Error in getting market with highest overlap for firm" << std::endl;
        throw std::exception;
    }

    return MiscUtils::choose_random_from_set(setMarketsWithHighestOverlap);
}

const set<int> &Firm::getSetMarketIDs() const {
    return setMarketIDs;
}
