//
// Created by Eric Thomas on 8/17/23.
//

#include "ControlAgent.h"
#include "../Utils/StringUtils.h"
#include <vector>
using std::endl;

#define NOT_YET_SET -1

ControlAgent::ControlAgent(const int &iAgentID, const string &strEntryPolicy,
                           const string &strExitPolicy, const double &dbEntryActionLikelihood,
                           const double &dbExitActionLikelihood, const double &dbNoneActionLikelihood,
                           const double &dbPercentThresholdForLossExitPolicy,
                           const int &iNumMicroStepsForLossExitPolicy) {
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
        this->iNumMicroStepsForLossExitPolicy = iNumMicroStepsForLossExitPolicy;
    }

    // Set the remaining agent hyperparameters
    this->iAgentID                = iAgentID;
    this->dbEntryActionLikelihood = dbEntryActionLikelihood;
    this->dbExitActionLikelihood  = dbExitActionLikelihood;
    this->dbNoneActionLikelihood  = dbNoneActionLikelihood;
    this->iFirmAssignment         = NOT_YET_SET;
}

// This method allows you to print the agent's details via cout << agent;
std::ostream &operator<<(std::ostream &os, const ControlAgent &agent) {
    os << "iAgentID: " << agent.iAgentID << endl
       << "enumEntryPolicy: " << agent.enumEntryPolicy << endl
       << "enumExitPolicy: " << agent.enumExitPolicy << endl
        << "dbEntryActionLikelihood: " << agent.dbEntryActionLikelihood << endl
        << "dbExitActionLikelihood: " << agent.dbExitActionLikelihood << endl
        << "dbNoneActionLikelihood: " << agent.dbNoneActionLikelihood << endl << endl;
    return os;
}

int ControlAgent::getAgentId() const {return iAgentID;}

// TODO: delete this when done brainstorming:
//  to get an agent's decision, we'll need to know
//  - all the agent decision-making params (this is easy because the decision function will be tied to the agent)
//  - the markets that the agent's firm is present in; we'll access this through the agent-to-firm ids
//  - the capability vector for each market we could enter (we'll pass in the economy, which contains all the market data)
//  - the simulator history, in order to calculate loss

Action ControlAgent::getAgentAction() {





}
