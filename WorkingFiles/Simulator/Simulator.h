//
// Created by Eric Thomas on 8/17/23.
//

#pragma once

#include <vector>
#include <map>
#include <vector>
#include <string>
#include "../Agent/ControlAgent.h"
#include "../Agent/StableBaselines3Agent.h"
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
    void init_master_history();
    void init_simulation_history();
    int init_data_cache(SimulationHistory* pCurrentSimulationHistory);
    int prepare_to_run();
    void set_agent_turn_order();
    vector<int> get_agent_turn_order();
    int reset();
    int run();
    int get_num_sims() const;
    int get_macro_steps_per_sim() const;
    int iCurrentSim = 0;
    int iCurrentMacroTimeStep = 0;
    int iCurrentMicroTimeStep = 0;
    MasterHistory masterHistory;
    bool bVerbose;
    int perform_micro_step(const int& iActingAgentID);
    int perform_micro_step(const int& iActingAgentID, const int& iAIAgentActionID);
    int perform_micro_step_helper(vector<Action> vecActions);
    int get_micro_steps_per_macro_step();
    bool is_ai_agent(const int& iAgentID);
    bool is_bankrupt(const int& iAgentID);
    vector<double> generate_state_observation(const int& iAgentID);
    vector<double> get_capital_representation(const int& iAgentID);
    vector<double> get_market_overlap_representation();
    vector<double> get_variable_cost_representation(const int& iAgentID);
    vector<double> get_fixed_cost_representation(const int& iAgentID);
    vector<double> get_market_portfolio_representation(const int& iAgentID);
    vector<double> get_entry_cost_representation(const int& iAgentID);
    vector<double> get_demand_intercept_representation();
    vector<double> get_demand_slope_representation();
    vector<double> get_quantity_representation(const int& iAgentID);
    vector<double> get_price_representation(const int& iAgentID);
    double generate_reward(const int& iAgentID);

private:
    nlohmann::json simulatorConfigs;
    map<int, BaseAgent*> mapAgentIDToAgentPtr;
    map<int, Firm*> mapFirmIDToFirmPtr;
    Economy economy;
    SimulationHistory* currentSimulationHistoryPtr;
    DataCache dataCache;
    vector<int> vecAgentTurnOrder;

    // Simulation parameters
    string strRunName;
    string strResultsDir;
    int iNumSims;
    int iMacroStepsPerSim;
    double dbSkippedTurnsPerRegularTurn;
    bool bRandomizeTurnOrderWithinEachMacroStep;
    bool bRandomizeAgentFirmAssignmentPerSimulation;
    bool bRandomizeVariableCostsPerSimulation;

    // Maps to track stats necessary for reward calculations
    map<int,int> mapAIAgentIDToMicroTimeStepOfLastTurn;
    map<int,double> mapAIAgentIDToCapitalAtLastTurn;

    // Helper for executing AI agent actions
    int iAgentIDOfMostRecentAITurn = -1;

    int init_control_agents();
    int init_AI_agents();
    int init_economy();
    int init_markets();
    int set_simulation_parameters();
    int init_firms_for_agents();
    vector<int> create_market_capability_vector(const double &dbMean, const double &dbSD);
    vector<Action> get_actions_for_all_agents(const int &iActingAgentID);
    vector<Action> get_actions_for_all_agents(const int &iActingAgentID, const int& iAIAgentActionID));
    Action convert_action_ID_to_action_object(const int& iActingAgentID, const int& iAIAgentActionID);
    int execute_actions(const vector<Action> &vecActions, map<int,double>* pMapFirmIDToCapitalChange);
    int execute_entry_action(const Action& action, map<int,double>* pMapFirmIDToCapitalChange);
    int execute_exit_action(const Action& action, map<int,double>* pMapFirmIDToCapitalChange);
    Action get_agent_action(const ControlAgent &agent);
    ActionType get_action_type(const ControlAgent &agent);
    Action get_entry_action(const ControlAgent &agent);
    Action get_exit_action(const ControlAgent &agent);
    int distribute_profits(map<int,double>* pMapFirmIDToCapitalChange);
    Firm* get_firm_ptr_from_agent_ptr(BaseAgent* agentPtr);
    Firm* get_firm_ptr_from_agent_id(const int& iAgentID);
    BaseAgent* get_agent_ptr_from_firm_ID(int iFirmID);
    Firm* get_firm_ptr_from_agent(const ControlAgent& agent);
    set<int> get_set_firm_IDs();
    set<int> get_set_market_IDs();
    set<int> get_firm_IDs_in_market(Market market);
    map<int,double> get_map_firm_to_var_cost_for_market(Market market);
    double get_average_var_cost_in_market(Market market);
    void add_profit_to_firm(double dbProfit, int iFirmID);
    void shuffle_agent_firm_assignments();
};