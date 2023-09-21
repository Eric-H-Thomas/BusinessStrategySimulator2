//
// Created by Eric Thomas on 8/17/23.
//

#pragma once

#include <vector>
#include <map>
#include <vector>
#include <string>
#include "../JSON/json.h"
#include "Agent/ControlAgent.h"
#include "Economy/Economy.h"
#include "Firm/Firm.h"
#include "History/MasterHistory.h"

using std::map;
using std::vector;

class Simulator {
public:
    Simulator();
    int load_json_configs(const string& strConfigFilePath);
    int prepare_to_run();
    int run();
    int getNumSims() const;
    int get_agent_to_firm_map(map<int,int>& rMapAgentToFirm);
    int perform_micro_step(int iActingAgentID);

private:
    nlohmann::json       simulatorConfigs;
    vector<ControlAgent> vecControlAgents;
    vector<Firm>         vecFirms;
    Economy              economy;
    MasterHistory        masterHistory;
    vector<int>          vecAgentTurnOrder;

    // Simulation parameters
    string      strRunName;
    string      strResultsDir;
    int         iNumSims;
    int         iMacroStepsPerSim;
    double      dbSkippedTurnsPerRegularTurn;
    bool        bVerbose;
    bool        bRandomizeTurnOrderWithinEachMacroStep;
    bool        bRandomizeAgentFirmAssignmentPerSimulation;

    int init_control_agents();
    int init_economy();
    int init_markets();
    int set_simulation_parameters();
    int init_firms_for_control_agents();
    vector<int> create_market_capability_vector(double dbMean, double dbSD);
    void set_agent_turn_order();
};
