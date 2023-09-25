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

class ControlAgent {
public:
    ControlAgent(const int &iAgentID, const string &strEntryPolicy,
                 const string &strExitPolicy, const double &dbEntryActionLikelihood,
                 const double &dbExitActionLikelihood, const double &dbNoneActionLikelihood,
                 const double &dbPercentThresholdForLossExitPolicy = 0.0,
                 const int &iNumMicroStepsForLossExitPolicy = 0);
    int iFirmAssignment;
    friend std::ostream &operator<<(std::ostream &os, const ControlAgent &agent);
    int getAgentId() const;
    double getDbEntryActionLikelihood() const;
    double getDbExitActionLikelihood() const;
    double getDbNoneActionLikelihood() const;
    std::vector<double> get_action_likelihood_vector() const;
    EntryPolicy getEnumEntryPolicy() const;
    ExitPolicy getEnumExitPolicy() const;

private:
    int iAgentID;
    EntryPolicy enumEntryPolicy;
    ExitPolicy enumExitPolicy;
    double dbEntryActionLikelihood;
    double dbExitActionLikelihood;
    double dbNoneActionLikelihood;
    double dbPercentThresholdForLossExitPolicy;
    int iNumMacroStepsForLossExitPolicy;
};



