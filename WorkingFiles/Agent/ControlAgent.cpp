//
// Created by Eric Thomas on 8/17/23.
//

#include "ControlAgent.h"
#include "../Utils/StringUtils.h"
#include <vector>
using std::endl;
using std::vector;

#define NOT_YET_SET -1

ControlAgent::ControlAgent(const int &iAgentID, const string &strEntryPolicy,
                           const string &strExitPolicy, const string &strProductionPolicy,
                           const double &dbEntryActionLikelihood,
                           const double &dbExitActionLikelihood, const double &dbNoneActionLikelihood,
                           const double &dbPercentThresholdForLossExitPolicy,
                           const int &iNumMacroStepsForLossExitPolicy) {
    // Set the entry policy
    if (StringUtils::equalsIgnoreCase(strEntryPolicy, "all"))
        this->enumEntryPolicy = EntryPolicy::All;
    if (StringUtils::equalsIgnoreCaseAndIgnoreUnderscores(strEntryPolicy, "highest_overlap"))
        this->enumEntryPolicy = EntryPolicy::HighestOverlap;

    // Set the exit policy
    if (StringUtils::equalsIgnoreCase(strExitPolicy, "all"))
        this->enumExitPolicy = ExitPolicy::All;
    if (StringUtils::equalsIgnoreCase(strExitPolicy, "loss")) {
        this->enumExitPolicy = ExitPolicy::Loss;
        this->dbPercentThresholdForLossExitPolicy = dbPercentThresholdForLossExitPolicy;
        this->iNumMacroStepsForLossExitPolicy = iNumMacroStepsForLossExitPolicy;
    }

    // Set the production policy
    if (StringUtils::equalsIgnoreCase(strProductionPolicy, "cournot")) {
        this->enumProductionPolicy = ProductionPolicy::Cournot;
    }

    // Set the remaining agent hyperparameters
    this->iAgentID                = iAgentID;
    this->dbEntryActionLikelihood = dbEntryActionLikelihood;
    this->dbExitActionLikelihood  = dbExitActionLikelihood;
    this->dbNoneActionLikelihood  = dbNoneActionLikelihood;
    this->iFirmAssignment         = NOT_YET_SET;
}

vector<double> ControlAgent::get_action_likelihood_vector() const {
    return {dbEntryActionLikelihood, dbExitActionLikelihood, dbNoneActionLikelihood};
}
int                 ControlAgent::getAgentId()              const {return iAgentID;}
EntryPolicy         ControlAgent::getEnumEntryPolicy()      const {return enumEntryPolicy;}
ExitPolicy          ControlAgent::getEnumExitPolicy()       const {return enumExitPolicy;}
ProductionPolicy    ControlAgent::getEnumProductionPolicy() const {return enumProductionPolicy;}
