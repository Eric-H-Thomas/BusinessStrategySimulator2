//
// Created by Eric Thomas on 8/17/23.
//

#pragma once

#include <vector>
#include <map>
#include <vector>
#include <string>
#include "../Agent/ControlAgent.h"
#include "../Economy/Economy.h"
#include "../Firm/Firm.h"
#include "../History/MasterHistory.h"
#include "../../JSONReader/json.h"
#include "../Utils/MiscUtils.h"
#include "../Market/Market.h"

using std::map;
using std::vector;

class Simulator {
public:
    Simulator();
    int load_json_configs(const string& strConfigFilePath);
    void init_simulation_history();
    int prepare_to_run();
    int run();
    int getNumSims() const;
    int perform_micro_step(int iActingAgentID);

private:
    nlohmann::json        simulatorConfigs;
    map<int,ControlAgent> mapIDToControlAgents;
    map<int,Firm>         mapIDToFirm;
    Economy               economy;
    MasterHistory         masterHistory;
    vector<int>           vecAgentTurnOrder;

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
    vector<Action> get_actions_for_all_control_agents(int iActingAgentID);
    Action get_agent_action(ControlAgent agent);
    ActionType get_action_type(ControlAgent agent);
    Action get_entry_action(ControlAgent agent);
    Action get_exit_action(ControlAgent agent);
    Firm get_firm_from_agent(ControlAgent agent);
};
