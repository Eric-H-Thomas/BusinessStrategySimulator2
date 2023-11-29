//
// Created by Eric Thomas on 8/17/23.
//

#pragma once
#include <string>
#include <ostream>
#include "../Action/Action.h"

using std::string;

enum class EntryPolicy {
    All,
    HighestOverlap
};

enum class ExitPolicy {
    All,
    Loss
};

enum class ProductionPolicy {
    Cournot
};

class ControlAgent {
public:
    ControlAgent(const int &iAgentID, const string &strEntryPolicy,
                 const string &strExitPolicy, const string &strProductionPolicy,
                 const double &dbEntryActionLikelihood,
                 const double &dbExitActionLikelihood, const double &dbNoneActionLikelihood,
                 const double &dbPercentThresholdForLossExitPolicy = 0.0,
                 const int &iNumMacroStepsForLossExitPolicy = 0);
    int iFirmAssignment;
    int getAgentId() const;
    std::vector<double> get_action_likelihood_vector() const;
    EntryPolicy getEnumEntryPolicy() const;
    ExitPolicy getEnumExitPolicy() const;
    ProductionPolicy getEnumProductionPolicy() const;
    string toString() const;

private:
    int iAgentID;
    EntryPolicy enumEntryPolicy;
    ExitPolicy enumExitPolicy;
    ProductionPolicy enumProductionPolicy;
    double dbEntryActionLikelihood;
    double dbExitActionLikelihood;
    double dbNoneActionLikelihood;
    double dbPercentThresholdForLossExitPolicy;
    int iNumMacroStepsForLossExitPolicy;
};



