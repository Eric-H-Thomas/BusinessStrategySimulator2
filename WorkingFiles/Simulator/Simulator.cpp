//
// Created by Eric Thomas on 8/17/23.
//

#include "Simulator.h"
#include "../Agent/ControlAgent.h"
#include <map>
#include <iostream>
#include "../../JSONReader/json.h"
#include "../Utils/MiscUtils.h"
#include "../Action/Action.h"
#include <fstream>
#include <iostream>
#include <random>
#include <cmath>

using std::map;
using std::vector;
using std::cerr;
using std::cout;
using std::endl;


int Simulator::run() {

    if (bVerbose) cout << "At the beginning of the Simulator::Run method" << endl;

    // TODO: make sure the agent to firm assignments get properly reset if they are not supposed to be the same for each
    //  round of simulation

    if (bVerbose) cout << "Initializing simulation history" << endl;
    init_simulation_history();

    if (bVerbose) cout << "Initializing data cache" << endl;
    init_data_cache(masterHistory.getCurrentSimulationHistoryPtr());

    // Loop through the macro steps
    if (bVerbose) cout << "Beginning loop through macro steps" << endl;

    for (int iMacroStep = 0; iMacroStep < iMacroStepsPerSim; iMacroStep++) {
        if (bVerbose) cout << "Beginning macro step " << iMacroStep + 1 << " of " << iMacroStepsPerSim << endl;
        if (bVerbose) cout << "Setting the agent turn order" << endl;

        set_agent_turn_order();
        for (int iAgentIndex : vecAgentTurnOrder) {
            if (bVerbose) cout << "Performing micro step. Acting agent ID: " << iAgentIndex << endl;
            if (perform_micro_step(iAgentIndex))
                return 1;
        }
    }

    return 0;
}

Simulator::Simulator() = default;

int Simulator::getNumSims() const {return iNumSims;}

int Simulator::load_json_configs(const string& strConfigFilePath) {

    // Read the JSONReader file
    std::ifstream file(strConfigFilePath);
    if (!file.is_open()) {
        cout << "Error reading json config file\n";
        return 1;
    }

    // Parse JSONReader data
    try {
        file >> simulatorConfigs;
    } catch (const nlohmann::json::exception& e) {
        // Handle JSONReader parsing error
        cerr << "JSONReader parsing error: " << e.what() << endl;
        return 1;
    }

    return 0;
};

int Simulator::prepare_to_run() {

    if (set_simulation_parameters())
        return 1;

    if (init_economy())
        return 1;

    if (init_markets())
        return 1;

    if (init_control_agents())
        return 1;

    if (init_firms_for_control_agents())
        return 1;

    return 0;
};

int Simulator::set_simulation_parameters() {
    try {
        const auto& simulation_parameters = this->simulatorConfigs["simulation_parameters"];

        this->strRunName = simulation_parameters["run_name"];
        this->strResultsDir = simulation_parameters["results_dir"];
        this->iNumSims = simulation_parameters["num_sims"];
        this->iMacroStepsPerSim = simulation_parameters["macro_steps_per_sim"];
        this->dbSkippedTurnsPerRegularTurn = simulation_parameters["skipped_turns_per_regular_turn"];
        this->bVerbose = simulation_parameters["verbose"];
        this->bRandomizeTurnOrderWithinEachMacroStep = simulation_parameters["randomize_turn_order_within_each_macro_step"];
        this->bRandomizeAgentFirmAssignmentPerSimulation = simulation_parameters["randomize_agent_firm_assignment_per_simulation"];
    }

    catch (const nlohmann::json::exception& e) {
        std::cerr << "Error extracting simulation parameters: " << e.what() << std::endl;
        return 1;
    }

    return 0;
};

int Simulator::init_economy() {

    try {
        const auto& economy_parameters = this->simulatorConfigs["default_economy_parameters"];
        const auto& market_parameters = this->simulatorConfigs["default_market_parameters"];

        vector<int> vecClusterMeans;
        for (const auto& clusterMean : economy_parameters["cluster_means"]) {
            vecClusterMeans.push_back(clusterMean);
        }

        vector<int> vecClusterSDs;
        for (const auto& clusterSD : economy_parameters["cluster_SDs"]) {
            vecClusterSDs.push_back(clusterSD);
        }

        vector<int> vecMarketsPerCluster;
        for (const auto& numMarkets : economy_parameters["markets_per_cluster"]) {
            vecMarketsPerCluster.push_back(numMarkets);
        }

        this->economy = Economy(economy_parameters["possible_capabilities"],
                                economy_parameters["capabilities_per_market"],
                                economy_parameters["total_markets"],
                                economy_parameters["firm_capacity"],
                                economy_parameters["num_market_clusters"],
                                vecClusterMeans,
                                vecClusterSDs,
                                vecMarketsPerCluster,
                                market_parameters["market_entry_cost_max"],
                                market_parameters["market_entry_cost_min"]);
    }

    catch (const nlohmann::json::exception& e) {
        std::cerr << "Error initializing economy: " << e.what() << std::endl;
        return 1;
    }

    return 0;
};

int Simulator::init_control_agents() {
    try {
        for (const auto& agentData : this->simulatorConfigs["agents"]) {
            auto agentPtr = new ControlAgent(agentData["agent_id"],
                                       agentData["entry_policy"],
                                       agentData["exit_policy"],
                                       agentData["production_policy"],
                                       agentData["entry_action_likelihood"],
                                       agentData["exit_action_likelihood"],
                                       agentData["none_action_likelihood"],
                                       agentData["percent_threshold_for_loss_exit_policy"],
                                       agentData["num_macro_steps_for_loss_exit_policy"]);

            this->mapAgentIDToAgentPtr.insert(std::make_pair(agentData["agent_id"], agentPtr));
        }
    }

    catch (const nlohmann::json::exception& e) {
        std::cerr << "Error initializing agents: " << e.what() << std::endl;
        return 1;
    }

    return 0;
};

int Simulator::init_firms_for_control_agents() {
    // For now, this assigns each agent a firm with
    //    - no capabilities
    //    - no presence in any markets
    //    - the default starting capital

    if (mapAgentIDToAgentPtr.empty()){
        cerr << "Tried to initialize firms for control agents before creating the control agents." << endl;
        return 1;
    }

    const auto& firm_parameters = this->simulatorConfigs["default_firm_parameters"];
    double dbDefaultStartingCapital = firm_parameters["starting_capital"];

    for (auto pair : mapAgentIDToAgentPtr) {
        // Get the ID number of the agent
        int iID = pair.first;

        // Create a firm with the same ID number and place it in the map of firms
        auto firmPtr = new Firm(iID, dbDefaultStartingCapital);
        mapFirmIDToFirmPtr.insert(std::make_pair(iID, firmPtr));

        // Set the agent's firm assignment number
        pair.second->iFirmAssignment = iID;
    }

    return 0;
}

int Simulator::init_markets() {
    try {
        const auto& market_parameters = this->simulatorConfigs["default_market_parameters"];

        double dbVarCostMin                 = market_parameters["variable_cost_min"];
        double dbVarCostMax                 = market_parameters["variable_cost_max"];
        double dbFixedCostPercentageOfEntry = market_parameters["fixed_cost_percentage_of_entry"];
        double dbDemandInterceptMax         = market_parameters["demand_intercept_max"];
        double dbDemandInterceptMin         = market_parameters["demand_intercept_min"];
        double dbProductDemandSlopeMax      = market_parameters["product_demand_slope_max"];
        double dbProductDemandSlopeMin      = market_parameters["product_demand_slope_min"];

        // Create a random number generator engine
        std::random_device rd;
        std::mt19937 gen(rd());

        for (int iCluster = 0; iCluster < this->economy.get_num_market_clusters(); iCluster++) {

            int iMarketsInCurrCluster = economy.get_vec_markets_per_cluster().at(iCluster);
            for (int j = 0; j < iMarketsInCurrCluster; j++){

                // Choose the market's var cost from a uniform distribution in the range [dbVarCostMin, dbVarCostMax)
                std::uniform_real_distribution<double> var_cost_dist(dbVarCostMin, dbVarCostMax);
                double dbVarCost = var_cost_dist(gen);

                // Choose the market's demand intercept from a uniform distribution in the range [dbDemandInterceptMin, dbDemandInterceptMax)
                std::uniform_real_distribution<double> demand_intercept_dist(dbDemandInterceptMin, dbDemandInterceptMax);
                double dbDemandIntercept = demand_intercept_dist(gen);

                // Choose the market's demand slope from a uniform distribution in the range [dbProductDemandSlopeMin, dbProductDemandSlopeMax)
                std::uniform_real_distribution<double> demand_slope_dist(dbProductDemandSlopeMin, dbProductDemandSlopeMax);
                double dbProductDemandSlope = demand_slope_dist(gen);

                // Create the capability vector for this market
                double dbMean = this->economy.get_vec_cluster_means().at(iCluster);
                double dbSD = this->economy.get_vec_cluster_SDs().at(iCluster);
                vector<int> vecMarketCapabilities = create_market_capability_vector(dbMean, dbSD);

                // Create a new market and add it to the economy's vector of markets
                this->economy.add_market(Market(dbFixedCostPercentageOfEntry, dbVarCost,

                                               dbDemandIntercept, dbProductDemandSlope, vecMarketCapabilities));
            } // End of inner loop
        } // End of outer loop
    } // End of try block
    catch (const std::exception& e) {
        cerr << "Error initializing agents: " << e.what() << endl;
        return 1;
    }
    return 0;
};

vector<int> Simulator::create_market_capability_vector(const double& dbMean, const double& dbSD) {
    // TODO: Test this method

    // Create a vector of all zeros
    vector<int> vecCapabilities(this->economy.get_num_possible_capabilities(), 0);

    // Create a normal distribution from the given mean and standard deviation
    std::normal_distribution<double> dist(dbMean, dbSD);

    // Create a random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // For capabilities_per_market times
    for (int i = 0; i < this->economy.get_num_capabilities_per_market(); i++) {
        // Get a random value from the distribution
        double dbRandomValue = dist(gen);

        // Round the random value to the nearest integer
        int iRoundedValue = std::round(dbRandomValue);

        // Keep track of whether we most recently rounded up or down to the nearest integer
        bool bMostRecentlyRoundedUp = false;
        if (iRoundedValue > dbRandomValue)
            bMostRecentlyRoundedUp = true;

        bool bDone = false;
        int iAttempts = 0;
        while (!bDone) {
            iAttempts++;
            // If the vector is defined at the index of that integer and contains a 0
            if (iRoundedValue >= 0 && iRoundedValue < vecCapabilities.size() && vecCapabilities.at(iRoundedValue) == 0) {
                vecCapabilities.at(iRoundedValue) = 1;
                bDone = true;
            }
            else {
                // Set the integer to its next value
                if (bMostRecentlyRoundedUp){
                    iRoundedValue = iRoundedValue - iAttempts;
                }
                else {
                    iRoundedValue = iRoundedValue + iAttempts;
                }
            }
        }
    } // End of for loop
    return vecCapabilities;
}

void Simulator::set_agent_turn_order() {
    // TODO: Test this method

    // If we are not resetting the order within each macro step, we only want to set the order once
    if (!bRandomizeTurnOrderWithinEachMacroStep && !vecAgentTurnOrder.empty())
        return;

    // TODO: edit this later for when there are non-control agents
    // Get the total number of turns
    double dbTotalTurns = mapAgentIDToAgentPtr.size() * dbSkippedTurnsPerRegularTurn;
    int iTotalTurns = static_cast<int>(std::ceil(dbTotalTurns));

    // Generate a vector for the new turn order
    vector<int> vecNewTurnOrder;
    for (int i = 1; i <= iTotalTurns; ++i) {
        vecNewTurnOrder.push_back(i);
    }

    // Shuffle the new turn order vector
    std::random_device rd;
    std::mt19937 generator(rd());
    std::shuffle(vecNewTurnOrder.begin(), vecNewTurnOrder.end(), generator);

    // Set the agent turn order equal to the newly created turn order
    vecAgentTurnOrder = vecNewTurnOrder;
};

int Simulator::perform_micro_step(const int& iActingAgentID) {
    iCurrentMicroTimeStep++;

    // TODO: edit this method to make it compatible with non-control agents
    // TODO: edit this method to work for the simultaneous model as well (or create an overload with no parameters)
    // TODO: make sure this method returns error codes as necessary

    if (bVerbose) cout << "Getting actions for all control agents" << endl;
    vector<Action> vecActions = get_actions_for_all_control_agents(iActingAgentID);

    if (bVerbose) cout << "Executing agent actions" << endl;
    execute_actions(vecActions);

    if (bVerbose) cout << "Distributing profits" << endl;
    if (bVerbose) cout << "(Note: Profit distribution not yet implemented)" << endl;
    // TODO: dist profits
    distribute_profits();

   return 0;
};

vector<Action> Simulator::get_actions_for_all_control_agents(const int& iActingAgentID) {
    vector<Action> vecActions;
    for (const auto& pair : mapAgentIDToAgentPtr) {
        auto agentPtr = pair.second;
        // Get action for the acting agent
        if (agentPtr->getAgentId() == iActingAgentID){
            vecActions.emplace_back(get_agent_action(*agentPtr));
        }
        else { // Create none actions for the agents not currently acting
            vecActions.emplace_back(Action::generate_none_action(agentPtr->getAgentId()));
        }
    }
    return vecActions;
}

void Simulator::execute_actions(const vector<Action>& vecActions) {
    for (const Action& action : vecActions) {
        switch (action.enumActionType) {
            case ActionType::enumEntryAction:
                execute_entry_action(action);
                break;
            case ActionType::enumExitAction:
                execute_exit_action(action);
                break;
            case ActionType::enumNoneAction:
                // Do nothing for ActionType::enumNoneAction
                break;
            default:
                // Should never reach this section of the code
                std::cerr << "Invalid enumerated action type" << std::endl;
                throw std::exception();
        }
    }
}

void Simulator::execute_entry_action(const Action& action) {
    auto agentPtr = mapAgentIDToAgentPtr.at(action.iAgentID);
    auto firmPtr = get_firm_ptr_from_agent_ptr(agentPtr);
    firmPtr->enter_market(action.iMarketID);
    currentSimulationHistoryPtr->record_market_presence_change(action.iMicroTimeStep, true, firmPtr->getFirmID(), action.iMarketID);
}

void Simulator::execute_exit_action(const Action& action) {
    auto agentPtr = mapAgentIDToAgentPtr.at(action.iAgentID);
    auto firmPtr = get_firm_ptr_from_agent_ptr(agentPtr);
    firmPtr->exit_market(action.iMarketID);
    currentSimulationHistoryPtr->record_market_presence_change(action.iMicroTimeStep, false, firmPtr->getFirmID(), action.iMarketID);
}

Action Simulator::get_agent_action(const ControlAgent& agent) {
    ActionType actionType = get_action_type(agent);

    if (actionType == ActionType::enumEntryAction) {
        return get_entry_action(agent);
    }
    else if (actionType == ActionType::enumExitAction) {
        return get_exit_action(agent);
    }
    else if (actionType == ActionType::enumNoneAction) {
        return Action::generate_none_action(agent.getAgentId());
    }

    // Should never reach this part of the code
    cerr << "Error getting control agent action" << endl;
    throw std::exception();
}

ActionType Simulator::get_action_type(const ControlAgent& agent) {
    int iActionTypeIndex = MiscUtils::choose_index_given_probabilities(agent.get_action_likelihood_vector());
    if (iActionTypeIndex == 0) {
        return ActionType::enumEntryAction;
    }
    if (iActionTypeIndex == 1) {
        return ActionType::enumExitAction;
    }
    if (iActionTypeIndex == 2) {
        return ActionType::enumNoneAction;
    }

    // Should never reach this part of the code
    cerr << "Error getting action_type" << endl;
    throw std::exception();
}

Action Simulator::get_entry_action(const ControlAgent& agent) {
    Firm* firmPtr = get_firm_ptr_from_agent(agent);
    set<Market> setPossibleMarketsToEnter;
    Market finalChoiceMarket;

    // Iterate through all markets, adding each one to the decision set if the firm is not already in that market
    for (const auto& market : economy.get_vec_markets()) {
        if (!firmPtr->is_in_market(market))
            setPossibleMarketsToEnter.insert(market);
    }

    // Check for the case that there are no markets to enter (extremely rare)
    if (setPossibleMarketsToEnter.empty()){
        return Action::generate_none_action(agent.getAgentId());
    }

    // Choose a market to enter based on the entry policy
    if (agent.getEnumEntryPolicy() == EntryPolicy::All) {
        finalChoiceMarket = MiscUtils::choose_random_from_set(setPossibleMarketsToEnter);
    }
    else if (agent.getEnumEntryPolicy() == EntryPolicy::HighestOverlap) {
        finalChoiceMarket = firmPtr->choose_market_with_highest_overlap(setPossibleMarketsToEnter);
    }
    else {
        // Should never reach this part of the code
        cerr << "Error getting action_type" << endl;
        throw std::exception();
    }

    // Construct and return the action object
    return Action(agent.getAgentId(), ActionType::enumEntryAction, finalChoiceMarket.get_market_id(), iCurrentMicroTimeStep);
}

Action Simulator::get_exit_action(const ControlAgent& agent) {
    Firm* firmPtr = get_firm_ptr_from_agent(agent);
    int iFinalChoiceMarketID;

    // Check for the case that there are no markets to exit
    if (firmPtr->getSetMarketIDs().empty()) {
        return Action::generate_none_action(agent.getAgentId());
    }

    // Choose a market to exit based on the exit policy
    if (agent.getEnumExitPolicy() == ExitPolicy::All) {
        iFinalChoiceMarketID = MiscUtils::choose_random_from_set(firmPtr->getSetMarketIDs());
    }

    // TODO: Add functionality for ExitPolicy::Loss
//    else if (agent.getEnumExitPolicy() == ExitPolicy::Loss) {
//
//    }

    else {
        // Should never reach this part of the code
        cerr << "Error getting action_type" << endl;
        throw std::exception();
    }

    // Construct and return the action object
    return Action(agent.getAgentId(), ActionType::enumExitAction, iFinalChoiceMarketID, iCurrentMicroTimeStep);
}

void Simulator::init_simulation_history() {
    // Generate map of agents to firms
    map<int,int> mapAgentToFirm;
    for (const auto& pair : this->mapAgentIDToAgentPtr) {
        auto agentPtr = pair.second;
        mapAgentToFirm[agentPtr->getAgentId()] = agentPtr->iFirmAssignment;
    }

    // Generate map of firms' starting capital amounts
    map<int,double> mapFirmStartingCapital;
    for (const auto& pair : this->mapFirmIDToFirmPtr) {
        auto firmPtr = pair.second;
        mapFirmStartingCapital[firmPtr->getFirmID()] = firmPtr->getDbCapital();
    }

    // Generate map of market maximum entry costs
    map<int,double> mapMarketMaximumEntryCosts;
    for (auto market : this->economy.get_vec_markets()){
        vector<int> vecMarketCapabilities = market.get_vec_capabilities();
        double dbMaxEntryCost = MiscUtils::dot_product(vecMarketCapabilities, this->economy.get_vec_capability_costs());
        mapMarketMaximumEntryCosts[market.get_market_id()] = dbMaxEntryCost;
    }

    // Initialize the simulation history using the above three maps
    currentSimulationHistoryPtr = new SimulationHistory(mapAgentToFirm, mapFirmStartingCapital, mapMarketMaximumEntryCosts);
    masterHistory.vecSimulationHistoryPtrs.push_back(currentSimulationHistoryPtr);
}

void Simulator::init_data_cache(SimulationHistory* pCurrentSimulationHistory) {
    // Initialize capital amounts
    dataCache.mapFirmCapital = pCurrentSimulationHistory->mapFirmStartingCapital; // TODO: test this to make sure it's getting passed by value, not reference

    // Get all firm-market combinations
    set<pair<int,int>> setFirmMarketCombinations;
    for (int iFirmID : get_set_firm_IDs()) {
        for (int iMarketID : get_set_market_IDs()) {
            setFirmMarketCombinations.insert(std::make_pair(iFirmID, iMarketID));
        }
    }

    // Initialize revenues, fixed costs, variable costs, and quantities produced to zero for each firm-market combination
    for (auto combination : setFirmMarketCombinations) {
        dataCache.mapFirmMarketComboToRevenue       [combination] = 0.0;
        dataCache.mapFirmMarketComboToFixedCost     [combination] = 0.0;
        dataCache.mapFirmMarketComboToVarCost       [combination] = 0.0;
        dataCache.mapFirmMarketComboToQtyProduced   [combination] = 0.0;
    }

    // Initialize each market-firm entry cost to the maximum entry cost for that market
    for (auto combination : setFirmMarketCombinations) {
        int iMarketID = combination.second;
        double dbEntryCost = pCurrentSimulationHistory->mapMarketMaximumEntryCost[iMarketID];
        dataCache.mapFirmMarketComboToEntryCost[combination] = dbEntryCost;
    }
}

Firm* Simulator::get_firm_ptr_from_agent_ptr(ControlAgent* agentPtr) {
    int iFirmID = agentPtr->iFirmAssignment;
    return mapFirmIDToFirmPtr.at(iFirmID);
}

Firm* Simulator::get_firm_ptr_from_agent(const ControlAgent& agent) {
    int iFirmID = agent.iFirmAssignment;
    return mapFirmIDToFirmPtr.at(iFirmID);
}

Firm* Simulator::get_firm_ptr_from_agent_id(const int& iAgentID) {
    ControlAgent* agentPtr = mapAgentIDToAgentPtr.at(iAgentID);
    return get_firm_ptr_from_agent_ptr(agentPtr);
}

void Simulator::distribute_profits() {
    // TODO: loop through each market, calculating the production level for each firm in the market


}

set<int> Simulator::get_set_firm_IDs() {
    set<int> setFirmIDs;
    for (auto pair : mapFirmIDToFirmPtr) {
        setFirmIDs.insert(pair.first);
    }
    return setFirmIDs;
}

set<int> Simulator::get_set_market_IDs() {
    return economy.get_set_market_IDs();
}
