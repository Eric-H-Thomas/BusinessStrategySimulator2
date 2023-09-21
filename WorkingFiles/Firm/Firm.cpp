//
// Created by Eric Thomas on 8/31/23.
//

#include "Firm.h"

Firm::Firm() = default;

Firm::Firm(int iFirmID, double dbStartingCapital) {
    this->iFirmID = iFirmID;
    this->dbCapital = dbStartingCapital;
}

int Firm::getIFirmId() const {
    return iFirmID;
}

double Firm::getDbCapital() const {
    return dbCapital;
}
