//
// Created by Eric Thomas on 8/17/23.
//

#include "Simulator.h"
#include <map>
#include <iostream>
#include <fstream>
#include <random>
#include <cmath>

using std::map;
using std::vector;
using std::cerr;
using std::cout;
using std::endl;


int Simulator::run() {

    init_simulation_history();

    if (init_data_cache(masterHistory.getCurrentSimulationHistoryPtr()))
        return 1;

    // Set current micro time step to 0 (needs to be reset between each run of the simulator)
    iCurrentMicroTimeStep = 0;

    // Loop through the macro steps
    for (int iMacroStep = 0; iMacroStep < iMacroStepsPerSim; iMacroStep++) {
        if (bVerbose) cout << "Beginning macro step " << iMacroStep + 1 << " of " << iMacroStepsPerSim << endl;

        set_agent_turn_order();

        // Loop through the micro steps
        for (int iAgentIndex : vecAgentTurnOrder) {
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
}

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

    init_master_history();

    return 0;
}

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
        this->bRandomizeVariableCostsPerSimulation = simulation_parameters["randomize_variable_costs_per_simulation"];
    }

    catch (const nlohmann::json::exception& e) {
        std::cerr << "Error extracting simulation parameters: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

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
}

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
}

int Simulator::init_firms_for_control_agents() {
    // For now, this assigns each agent a firm with
    //    - no capabilities
    //    - no presence in any markets
    //    - the default starting capital

    if (mapAgentIDToAgentPtr.empty()) {
        cerr << "Tried to initialize firms for control agents before creating the control agents." << endl;
        return 1;
    }

    const auto& firm_parameters = this->simulatorConfigs["default_firm_parameters"];
    double dbDefaultStartingCapital = firm_parameters["starting_capital"];
    const auto& economy_parameters = this->simulatorConfigs["default_economy_parameters"];
    int iPossibleCapabilities = economy_parameters["possible_capabilities"];

    for (auto pair : mapAgentIDToAgentPtr) {
        // Get the ID number of the agent
        int iID = pair.first;

        // Create a firm with the same ID number and place it in the map of firms
        auto firmPtr = new Firm(iID, dbDefaultStartingCapital, iPossibleCapabilities);
        mapFirmIDToFirmPtr.insert(std::make_pair(iID, firmPtr));

        // Set the agent's firm assignment number
        pair.second->iFirmAssignment = iID;
    }

    if (bRandomizeAgentFirmAssignmentPerSimulation) {
        shuffle_agent_firm_assignments();
    }

    return 0;
}

void Simulator::init_master_history() {
    // Get the number of micro steps per macro step
    double dbMicroStepsPerMacroStep = mapAgentIDToAgentPtr.size() * (1.0 + dbSkippedTurnsPerRegularTurn);
    int iMicroStepsPerMacroStep = static_cast<int>(std::ceil(dbMicroStepsPerMacroStep));

    masterHistory.iMicroStepsPerSim = iMicroStepsPerMacroStep * iMacroStepsPerSim;
    masterHistory.iNumFirms = mapFirmIDToFirmPtr.size();
    masterHistory.iNumMarkets = economy.get_total_markets();
}

void Simulator::shuffle_agent_firm_assignments() {
    cerr << "Called shuffle agent-firm assignments option.\nThis has not been implemented yet because the simulator "\
    "currently creates identical firms that begin with the same capital and no capabilities.\nCome back and fill in "\
    "this function or turn the shuffle option off." << endl;
    throw std::exception();
}

int Simulator::init_markets() {
    try {
        const auto& market_parameters = this->simulatorConfigs["default_market_parameters"];
        double dbVarCostMin                 = market_parameters["variable_cost_min"];
        double dbVarCostMax                 = market_parameters["variable_cost_max"];
        double dbFixedCostPercentageOfEntry = market_parameters["fixed_cost_percentage_of_entry"];
        double dbExitCostPercentageOfEntry  = market_parameters["exit_cost_percentage_of_entry"];
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
                this->economy.add_market(Market(dbFixedCostPercentageOfEntry, dbExitCostPercentageOfEntry,
                                               dbDemandIntercept, dbProductDemandSlope, vecMarketCapabilities));
            } // End of inner loop
        } // End of outer loop
    } // End of try block
    catch (const std::exception& e) {
        cerr << "Error initializing markets: " << e.what() << endl;
        return 1;
    }
    return 0;
}

vector<int> Simulator::create_market_capability_vector(const double& dbMean, const double& dbSD) {
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
    if (bVerbose) cout << "Setting the agent turn order" << endl;

    // If we are not resetting the order within each macro step, we only want to set the order once
    if (!bRandomizeTurnOrderWithinEachMacroStep && !vecAgentTurnOrder.empty())
        return;

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
}

int Simulator::perform_micro_step(const int& iActingAgentID) {
    if (bVerbose) cout << "Performing micro step. Acting agent ID: " << iActingAgentID << endl;

    iCurrentMicroTimeStep++;

    // Get agent actions
    vector<Action> vecActions;
    try {
        vecActions = get_actions_for_all_control_agents(iActingAgentID);
    }
    catch (std::exception e) {
        cerr << "Error getting actions for control agents during micro step " << iCurrentMicroTimeStep << endl;
        cerr << e.what() << endl;
        return 1;
    }

    // Create a map of capital change for each firm within this micro step (capital can be affected by both action
    // execution and profit distribution, so to get the total capital change within the micro time step we must add
    // these two effects). Initialize all values to zero.
    map<int,double> mapFirmIDToCapitalChange;
    for (auto firmID : get_set_firm_IDs()) {
        mapFirmIDToCapitalChange[firmID] = 0.0;
    }

    // Execute actions and distribute profits
    if (execute_actions(vecActions, &mapFirmIDToCapitalChange))
        return 1;

    if (distribute_profits(&mapFirmIDToCapitalChange))
        return 1;

    // Record capital changes in the history
    for (auto pair : mapFirmIDToCapitalChange) {
        if (pair.second != 0.0) {
            int iFirmID = pair.first;
            auto pFirm = mapFirmIDToFirmPtr[iFirmID];
            double dbCapital = pFirm->getDbCapital();
            currentSimulationHistoryPtr->record_capital_change(iCurrentMicroTimeStep, iFirmID, dbCapital);
        }
    }

    return 0;
}

vector<Action> Simulator::get_actions_for_all_control_agents(const int& iActingAgentID) {
    if (bVerbose) cout << "Getting actions for all control agents" << endl;

    vector<Action> vecActions;
    for (const auto& pair : mapAgentIDToAgentPtr) {
        auto agentPtr = pair.second;
        // Get action for the acting agent
        if (agentPtr->getAgentId() == iActingAgentID) {
            try {
                vecActions.emplace_back(get_agent_action(*agentPtr));
            }
            catch (std::exception e){
                cerr << "Error getting actions for control agents" << e.what() << endl;
                throw std::exception();
            }
        }
        else { // Create none actions for the agents not currently acting
            vecActions.emplace_back(Action::generate_none_action(agentPtr->getAgentId()));
        }
    }
    return vecActions;
}

int Simulator::execute_actions(const vector<Action>& vecActions, map<int,double>* pMapFirmIDToCapitalChange) {
    if (bVerbose) cout << "Executing agent actions" << endl;

    for (const Action& action : vecActions) {
        switch (action.enumActionType) {
            case ActionType::enumEntryAction:
                if (execute_entry_action(action, pMapFirmIDToCapitalChange))
                    return 1;
                break;
            case ActionType::enumExitAction:
                if (execute_exit_action(action, pMapFirmIDToCapitalChange))
                    return 1;
                break;
            case ActionType::enumNoneAction:
                // Do nothing for ActionType::enumNoneAction
                break;
            default:
                // Should never reach this section of the code
                std::cerr << "Invalid enumerated action type" << std::endl;
                return 1;
        } // End of switch block
    } // End of for loop

    return 0;
}

int Simulator::execute_entry_action(const Action& action, map<int,double>* pMapFirmIDToCapitalChange) {
    // Get pointer to the firm
    auto agentPtr = mapAgentIDToAgentPtr.at(action.iAgentID);
    auto firmPtr = get_firm_ptr_from_agent_ptr(agentPtr);

    auto pairFirmMarket = std::make_pair(firmPtr->getFirmID(), action.iMarketID);

    // Get the entry cost for the firm
    double dbEntryCost = dataCache.mapFirmMarketComboToEntryCost.at(pairFirmMarket);

    // Update the map of firm IDs to capital change with the entry cost of this action
    pMapFirmIDToCapitalChange->at(firmPtr->getFirmID()) -= dbEntryCost;

    // Update the fixed cost for this firm-market combo
    auto marketCopy = economy.get_market_by_ID(action.iMarketID);
    double dbFixedCostAsPctOfEntryCost = marketCopy.getFixedCostAsPercentageOfEntryCost();
    dbFixedCostAsPctOfEntryCost *= 0.01; // Scaling factor for percentage expressed as whole number
    double dbFixedCost = dbFixedCostAsPctOfEntryCost * dbEntryCost;
    dataCache.mapFirmMarketComboToFixedCost[pairFirmMarket] = dbFixedCost;
    currentSimulationHistoryPtr->record_fixed_cost_change(iCurrentMicroTimeStep,
                                                          dbFixedCost, pairFirmMarket.first, pairFirmMarket.second);

    // Update the firm's capability vector
    firmPtr->add_market_capabilities_to_firm_capabilities(economy.get_market_by_ID(action.iMarketID));

    // Add this market to the firm's portfolio and record this change in the history
    if (firmPtr->add_market_to_portfolio(action.iMarketID))
        return 1;
    currentSimulationHistoryPtr->record_market_presence_change(action.iMicroTimeStep, true, firmPtr->getFirmID(), action.iMarketID);

    // Update the entry cost for this firm for other markets
    for (Market market : economy.get_vec_markets()) {
        // Get vector of capabilities the firm lacks to enter the market
        auto vecFirmCapabilities = firmPtr->getVecCapabilities();
        auto vecMarketCapabilities = market.get_vec_capabilities();

        if (vecFirmCapabilities.size() != vecMarketCapabilities.size()) {
            cerr << "Mismatch between firm capability vector size and market capability vector size in execute_entry_action()" << endl;
            return 1;
        }

        std::vector<int> vecMissingCapabilities;
        // Reserve space for the missing capabilities vector
        vecMissingCapabilities.reserve(vecFirmCapabilities.size());

        // Set the vecMissingCapabilities vector to 1 where the market requires a capability the firm does not have
        for (size_t i = 0; i < vecFirmCapabilities.size(); i++) {
            if (vecMarketCapabilities[i] && !vecFirmCapabilities[i]) {
                vecMissingCapabilities.push_back(1);
            }
            else {
                vecMissingCapabilities.push_back(0);
            }
        }

        // Calculate the cost of the missing capabilities vector
        double dbCost = MiscUtils::dot_product(vecMissingCapabilities, economy.get_vec_capability_costs());

        // Update the data cache and the history if the entry cost has changed since it was last calculated
        double dbPriorCost = dataCache.mapFirmMarketComboToEntryCost[pairFirmMarket];
        if (dbCost != dbPriorCost) {
            dataCache.mapFirmMarketComboToEntryCost[pairFirmMarket] = dbCost;
            currentSimulationHistoryPtr->record_entry_cost_change(iCurrentMicroTimeStep,
                                                                  dbCost, firmPtr->getFirmID(), market.get_market_id());
        }
    }

    return 0;
}

int Simulator::execute_exit_action(const Action& action, map<int,double>* pMapFirmIDToCapitalChange) {
    // Get pointer to the firm
    auto agentPtr = mapAgentIDToAgentPtr.at(action.iAgentID);
    auto firmPtr = get_firm_ptr_from_agent_ptr(agentPtr);

    auto pairFirmMarket = std::make_pair(firmPtr->getFirmID(), action.iMarketID);

    // Get a copy of the market
    auto marketCopy = economy.get_market_by_ID(action.iMarketID);

    // Get the exit cost for the firm
    double dbEntryCost = dataCache.mapFirmMarketComboToEntryCost.at(pairFirmMarket);
    double dbExitCost = marketCopy.getExitCostAsPercentageOfEntryCost() * dbEntryCost * 0.01; // Scaling factor due to whole percentages

    // Update the map of firm IDs to capital change with the entry cost of this action
    pMapFirmIDToCapitalChange->at(firmPtr->getFirmID()) -= dbExitCost;

    // Update the fixed cost for this firm-market combo
    dataCache.mapFirmMarketComboToFixedCost[pairFirmMarket] = 0.0;
    currentSimulationHistoryPtr->record_fixed_cost_change(iCurrentMicroTimeStep,
                                                          0.0, pairFirmMarket.first, pairFirmMarket.second);

    // Update the firm's capability vector
    firmPtr->remove_market_capabilities_from_firm_capabilities(marketCopy, economy);

    // Remove this market from the firm's portfolio and record this change in the history
    if (firmPtr->remove_market_from_portfolio(action.iMarketID))
        return 1;
    currentSimulationHistoryPtr->record_market_presence_change(action.iMicroTimeStep, false, firmPtr->getFirmID(), action.iMarketID);

    // Update the entry cost for this firm for each market
    for (Market market : economy.get_vec_markets()) {
        // Get vector of capabilities the firm lacks to enter the market
        auto vecFirmCapabilities = firmPtr->getVecCapabilities();
        auto vecMarketCapabilities = market.get_vec_capabilities();

        if (vecFirmCapabilities.size() != vecMarketCapabilities.size()) {
            cerr << "Mismatch between firm capability vector size and market capability vector size in execute_entry_action()" << endl;
            return 1;
        }

        std::vector<int> vecMissingCapabilities;
        // Reserve space for the missing capabilities vector
        vecMissingCapabilities.reserve(vecFirmCapabilities.size());

        // Set the vecMissingCapabilities vector to 1 where the market requires a capability the firm does not have
        for (size_t i = 0; i < vecFirmCapabilities.size(); i++) {
            if (vecMarketCapabilities[i] && !vecFirmCapabilities[i]) {
                vecMissingCapabilities.push_back(1);
            }
            else {
                vecMissingCapabilities.push_back(0);
            }
        }

        // Calculate the cost of the missing capabilities vector
        double dbCost = MiscUtils::dot_product(vecMissingCapabilities, economy.get_vec_capability_costs());

        // Update the data cache and the history if the entry cost has changed since it was last calculated
        double dbPriorCost = dataCache.mapFirmMarketComboToEntryCost[pairFirmMarket];
        if (dbCost != dbPriorCost) {
            dataCache.mapFirmMarketComboToEntryCost[pairFirmMarket] = dbCost;
            currentSimulationHistoryPtr->record_entry_cost_change(iCurrentMicroTimeStep,
                                                                  dbCost, firmPtr->getFirmID(), market.get_market_id());
        }
    }

    return 0;
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
    if (bVerbose) cout << "Initializing simulation history" << endl;

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
    for (auto market : this->economy.get_vec_markets()) {
        vector<int> vecMarketCapabilities = market.get_vec_capabilities();
        double dbMaxEntryCost = MiscUtils::dot_product(vecMarketCapabilities, this->economy.get_vec_capability_costs());
        mapMarketMaximumEntryCosts[market.get_market_id()] = dbMaxEntryCost;
    }

    // Initialize the simulation history using the above three maps
    currentSimulationHistoryPtr = new SimulationHistory(mapAgentToFirm, mapFirmStartingCapital, mapMarketMaximumEntryCosts);
    masterHistory.vecSimulationHistoryPtrs.push_back(currentSimulationHistoryPtr);
}

int Simulator::init_data_cache(SimulationHistory* pCurrentSimulationHistory) {
    if (bVerbose) cout << "Initializing data cache" << endl;

    try {
        // Get all firm-market combinations
        set<pair<int,int>> setFirmMarketCombinations;
        for (int iFirmID : get_set_firm_IDs()) {
            for (int iMarketID : get_set_market_IDs()) {
                setFirmMarketCombinations.insert(std::make_pair(iFirmID, iMarketID));
            }
        }

        // Create a uniform distribution for drawing variable costs in the range [dbVarCostMin, dbVarCostMax)
        const auto& default_market_parameters = this->simulatorConfigs["default_market_parameters"];
        double dbVarCostMin = default_market_parameters["variable_cost_min"];
        double dbVarCostMax = default_market_parameters["variable_cost_max"];
        std::uniform_real_distribution<double> var_cost_dist(dbVarCostMin, dbVarCostMax);

        // Create a random number generator engine
        std::random_device rd;
        std::mt19937 gen(rd());

        // Initialize revenues, fixed costs, quantities produced to zero for each firm-market combination.
        // Initialize variable costs using the uniform distribution created above.
        for (auto combination : setFirmMarketCombinations) {
            dataCache.mapFirmMarketComboToRevenue       [combination] = 0.0;
            dataCache.mapFirmMarketComboToFixedCost     [combination] = 0.0;
            dataCache.mapFirmMarketComboToQtyProduced   [combination] = 0.0;

            // (Reuse the variable costs from the previous simulation if this condition is false)
            if (bRandomizeVariableCostsPerSimulation && dataCache.bInitialized) {
                dataCache.mapFirmMarketComboToVarCost   [combination] = var_cost_dist(gen);
            }
        }

        // Initialize each market-firm entry cost to the maximum entry cost for that market
        for (auto combination : setFirmMarketCombinations) {
            int iMarketID = combination.second;
            double dbEntryCost = pCurrentSimulationHistory->mapMarketMaximumEntryCost[iMarketID];
            dataCache.mapFirmMarketComboToEntryCost[combination] = dbEntryCost;
        }

        // Mark the data cache as having been initialized
        dataCache.bInitialized = true;

    } // End of try block

    catch (const nlohmann::json::exception& e) {
        std::cerr << "Error initializing the data cache: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

Firm* Simulator::get_firm_ptr_from_agent_ptr(ControlAgent* agentPtr) {
    int iFirmID = agentPtr->iFirmAssignment;
    return mapFirmIDToFirmPtr.at(iFirmID);
}

ControlAgent* Simulator::get_agent_ptr_from_firm_ID(int iFirmID) {
    for (auto pair : mapAgentIDToAgentPtr) {
        auto pAgent = pair.second;
        if (pAgent->iFirmAssignment == iFirmID) {
            return pAgent;
        }
    }
    cerr << "Error in get_agent_ptr_from_firm_ID" << endl;
    throw std::exception();
}

Firm* Simulator::get_firm_ptr_from_agent(const ControlAgent& agent) {
    int iFirmID = agent.iFirmAssignment;
    return mapFirmIDToFirmPtr.at(iFirmID);
}

int Simulator::distribute_profits(map<int,double>* pMapFirmIDToCapitalChange) {
    if (bVerbose) cout << "Distributing profits" << endl;

    // Iterate through each of the markets in the economy
    for (auto market : economy.get_vec_markets()) {
        /*
            Here, we use the same variable names provided in the simulator documentation:
              Key:
              Q: total production
              P: price level (for now, we assume one price level for all firms)
              q: production for a specific firm-market combination
              n: number of firms in the market
              a: demand curve intercept
              b: demand curve slope
              V: average variable cost in the market
              v: variable cost for a specific firm-market combination
        */

        // Calculate the total production level and price for the market
        set<int> setFirmIDsInMarket = get_firm_IDs_in_market(market);
        double n = (double) setFirmIDsInMarket.size(); // Cast to double to avoid integer division problems
        double V = get_average_var_cost_in_market(market);
        double a = market.getDbDemandIntercept();
        double b = market.getDbDemandSlope();
        double Q = (n / (n+1.0) ) * ( (a-V) / b);
        double P = a - (b*Q);

        auto mapFirmIDToVarCosts = get_map_firm_to_var_cost_for_market(market);

        // Iterate through the firms in the current market
        for (int iFirmID : get_firm_IDs_in_market(market)) {
            // Calculate production quantity for the firm-market combo
            double v = mapFirmIDToVarCosts[iFirmID];

            ProductionPolicy policy;
            try {
                auto pAgent = get_agent_ptr_from_firm_ID(iFirmID);
                policy = pAgent->getEnumProductionPolicy();
            }
            catch (std::exception e) {
                cerr << "Error in distribute_profits method: " << e.what() << endl;
                return 1;
            }

            double q; // Production quantity for this firm-market combo
            if (policy == ProductionPolicy::Cournot) {
                q = (a - (b*Q) - v) / b;
            }
            else {
                cerr << "Did not specify a valid production policy" << endl;
                return 1;
            }

            // Calculate revenue and profit for the firm-market combo
            auto pairFirmMarket = std::make_pair(iFirmID, market.get_market_id());
            double dbRevenue = q * P;
            double dbFixedCost = dataCache.mapFirmMarketComboToFixedCost[pairFirmMarket];
            double dbVarCost = q * v;
            double dbProfit = dbRevenue - dbFixedCost - dbVarCost;

            // Update capital within the firm object
            add_profit_to_firm(dbProfit, iFirmID);

            // Track accumulated profit for this firm for this time step
            pMapFirmIDToCapitalChange->at(iFirmID) += dbProfit;

            // Update revenue in the history and data cache if needed
            if (dbRevenue != dataCache.mapFirmMarketComboToRevenue[pairFirmMarket]) {
                currentSimulationHistoryPtr->record_revenue_change(iCurrentMicroTimeStep, dbRevenue,
                                                                   pairFirmMarket.first, pairFirmMarket.second);
                dataCache.mapFirmMarketComboToRevenue[pairFirmMarket] = dbRevenue;
            }

            // Update production quantities in the history and data cache if needed
            if (q != dataCache.mapFirmMarketComboToQtyProduced[pairFirmMarket]) {
                currentSimulationHistoryPtr->record_production_quantity_change(iCurrentMicroTimeStep, q,
                                                                   pairFirmMarket.first, pairFirmMarket.second);
                dataCache.mapFirmMarketComboToQtyProduced[pairFirmMarket] = q;
            }
        } // End of loop through firms
    } // End of loop through markets
    return 0;
} // End of distribute_profits method

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

set<int> Simulator::get_firm_IDs_in_market(Market market) {
    set<int> setFirmIDs;
    for (auto pair : mapFirmIDToFirmPtr) {
        auto pFirm = pair.second;
        if (pFirm->is_in_market(market)) {
            setFirmIDs.insert(pFirm->getFirmID());
        }
    }
   return setFirmIDs;
}

map<int,double> Simulator::get_map_firm_to_var_cost_for_market(Market market) {
    map<int,double> mapFirmToVarCost;
    int iMarketID = market.get_market_id();

    for (auto pair : dataCache.mapFirmMarketComboToVarCost) {
        auto currentFirmMarketPair = pair.first;
        double dbCurrentVarCost = pair.second;

        int iCurrentFirmID = currentFirmMarketPair.first;
        int iCurrentMarketID = currentFirmMarketPair.second;

        if (iMarketID == iCurrentMarketID) {
            mapFirmToVarCost.insert(std::make_pair(iCurrentFirmID, dbCurrentVarCost));
        }
    }

    return mapFirmToVarCost;
}

double Simulator::get_average_var_cost_in_market(Market market) {
    map<int,double> mapFirmToVarCost = get_map_firm_to_var_cost_for_market(market);

    double dbTotalVarCost = 0.0;

    for (auto pair : mapFirmToVarCost) {
        dbTotalVarCost += pair.second;
    }

    int iTotalFirms = mapFirmToVarCost.size();
    return dbTotalVarCost / iTotalFirms;
}

void Simulator::add_profit_to_firm(double dbProfit, int iFirmID) {
    auto pFirm = mapFirmIDToFirmPtr[iFirmID];
    pFirm->add_capital(dbProfit);
}
