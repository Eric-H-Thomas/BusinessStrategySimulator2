//
// Created by Eric Thomas on 8/31/23.
//

#pragma once
#include <set>

using std::set;

class Firm {
public:
    Firm();
    Firm(int iFirmID, double dbStartingCapital);
    int getIFirmId() const;
    double getDbCapital() const;

private:
    int         iFirmID;
    double      dbCapital;
    set<int>    setCapabilities;
    set<int>    setMarketIDs;
};
