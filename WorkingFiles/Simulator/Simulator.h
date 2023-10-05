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
#include "../DataCache/DataCache.h"

using std::map;
using std::vector;

class Simulator {
public:
    Simulator();
    int load_json_configs(const string &strConfigFilePath);
    void init_simulation_history();
    void init_data_cache(SimulationHistory* pCurrentSimulationHistory);
    int prepare_to_run();
    int run();
    int getNumSims() const;

private:
    nlohmann::json simulatorConfigs;
    map<int, ControlAgent*> mapAgentIDToAgentPtr;
    map<int, Firm*> mapFirmIDToFirmPtr;
    Economy economy;
    SimulationHistory* currentSimulationHistoryPtr;
    MasterHistory masterHistory;
    DataCache dataCache;
    vector<int> vecAgentTurnOrder;

    // Simulation parameters
    string strRunName;
    string strResultsDir;
    int iNumSims;
    int iMacroStepsPerSim;
    int iCurrentMicroTimeStep = 0;
    double dbSkippedTurnsPerRegularTurn;
    bool bVerbose;
    bool bRandomizeTurnOrderWithinEachMacroStep;
    bool bRandomizeAgentFirmAssignmentPerSimulation;

    int init_control_agents();
    int init_economy();
    int init_markets();
    int set_simulation_parameters();
    int init_firms_for_control_agents();
    vector<int> create_market_capability_vector(const double &dbMean, const double &dbSD);
    void set_agent_turn_order();
    int perform_micro_step(const int& iActingAgentID);
    vector<Action> get_actions_for_all_control_agents(const int &iActingAgentID);
    void execute_actions(const vector<Action> &vecActions);
    void execute_entry_action(const Action& action);
    void execute_exit_action(const Action& action);
    Action get_agent_action(const ControlAgent &agent);
    ActionType get_action_type(const ControlAgent &agent);
    Action get_entry_action(const ControlAgent &agent);
    Action get_exit_action(const ControlAgent &agent);
    void distribute_profits();
    Firm* get_firm_ptr_from_agent_ptr(ControlAgent* agentPtr);
    Firm* get_firm_ptr_from_agent(const ControlAgent& agent);
    Firm* get_firm_ptr_from_agent_id(const int& iAgentID);
    set<int> get_set_firm_IDs();
    set<int> get_set_market_IDs();
};