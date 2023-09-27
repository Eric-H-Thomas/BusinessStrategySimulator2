//
// Created by Eric Thomas on 8/31/23.
//

#pragma once
#include <set>
#include "../Market/Market.h"

using std::set;

class Firm {
public:
    Firm();
    Firm(int iFirmID, double dbStartingCapital);
    int getFirmID() const;
    double getDbCapital() const;
    bool is_in_market(Market market);
    Market choose_market_with_highest_overlap(set<Market> setMarkets);
    const set<int> &getSetMarketIDs() const;

private:
    int         iFirmID;
    double      dbCapital;
    vector<int> vecCapabilities;
    set<int>    setMarketIDs;
};
