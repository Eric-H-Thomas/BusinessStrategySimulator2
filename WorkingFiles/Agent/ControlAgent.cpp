//
// Created by Eric Thomas on 8/17/23.
//

#include "ControlAgent.h"
#include "../Utils/StringUtils.h"
#include <vector>
#include <iostream>
using std::endl;
using std::vector;
using std::cerr;

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

// Getters
int                 ControlAgent::getAgentId()              const {return iAgentID;}
EntryPolicy         ControlAgent::getEnumEntryPolicy()      const {return enumEntryPolicy;}
ExitPolicy          ControlAgent::getEnumExitPolicy()       const {return enumExitPolicy;}
ProductionPolicy    ControlAgent::getEnumProductionPolicy() const {return enumProductionPolicy;}

string ControlAgent::toString() const {
    // Agent ID number
    string output = "ID:";
    output += std::to_string(iAgentID);

    // Entry policy
    output += "__";
    output += "Entry Policy:";
    if (enumEntryPolicy == EntryPolicy::All) {
        output += "All";
    }
    else if (enumEntryPolicy == EntryPolicy::HighestOverlap) {
        output += "HighestOverlap";
    }
    else {
        throw std::exception();
        cerr << "Haha nice try bud. Entry policy not yet configured in control agent toString method" << endl;
    }

    // Exit policy
    output += "__";
    output += "Exit Policy:";
    if (enumExitPolicy == ExitPolicy::All) {
        output += "All";
    }
    else if (enumExitPolicy == ExitPolicy::Loss) {
        output += "Loss";
    }
    else {
        throw std::exception();
        cerr << "Haha nice try bud. Exit policy not yet configured in control agent toString method" << endl;
    }

    // Production policy
    output += "__";
    output += "Production Policy:";
    if (enumProductionPolicy == ProductionPolicy::Cournot) {
        output += "Cournot";
    }
    else {
        throw std::exception();
        cerr << "Haha nice try bud. Production policy not yet configured in control agent toString method" << endl;
    }

    // Entry action likelihood
    output += "__";
    output += "Entry action likelihood:";
    output += std::to_string(dbEntryActionLikelihood);

    // Exit action likelihood
    output += "__";
    output += "Exit action likelihood:";
    output += std::to_string(dbExitActionLikelihood);

    // None action likelihood
    output += "__";
    output += "None action likelihood:";
    output += std::to_string(dbNoneActionLikelihood);

    // Percent threshold for loss exit policy
    output += "__";
    output += "Percent threshold for loss exit policy:";
    output += std::to_string(dbPercentThresholdForLossExitPolicy);

    // Number of macro steps for exit loss policy
    output += "__";
    output += "Number of macro steps for exit loss policy:";
    output += std::to_string(iNumMacroStepsForLossExitPolicy);

    return output;
}
