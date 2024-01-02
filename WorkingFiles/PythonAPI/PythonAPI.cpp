//
// Created by Eric Thomas on 12/5/23.
//

#include "PythonAPI.h"

int PythonAPI::init_simulator(string strJsonConfigs) {
    // TODO: write code to check that all values in the JSON config file are valid

    if (simulator.load_json_configs(strJsonConfigs))
        return 1;

    if (simulator.prepare_to_run())
        return 1;

    return 0;
}

// This function assumes that init_simulator has already been called
vector<double> PythonAPI::reset() {
    // Reset the simulator to prepare it for the start of a new simulation
    if (simulator.reset()) {
        cerr << "Error occurred in PythonAPI::reset()" << endl;
        throw std::exception();
    }

    // Step the simulator until the first AI agent turn is reached
    auto tupleObsRewardTermTrunc = step_helper();

    // Unpack the state observation from the tuple generated by the step function
    return std::get<0>(tupleObsRewardTermTrunc);
}

// TODO: implement error checking; was using integer error codes but had to change this
// Returns tuple containing observation, reward, terminated, truncated
tuple<vector<double>, double, bool, bool> PythonAPI::step(int iActionID) {
    // Execute the given AI agent action
    if (simulator.get_num_AI_agents() > 0) {
        int iNextAIAgentIndex = simulator.get_next_AI_agent_index();
        simulator.perform_micro_step_ai_agent_turn(iNextAIAgentIndex, iActionID);
    }

    return step_helper();
}

tuple<vector<double>, double, bool, bool> PythonAPI::step_helper() {
    // Continue running the simulator until we encounter the next AI agent turn or reach the end of the simulation.
    // Loop through the macro steps
    while (simulator.iCurrentMacroTimeStep < simulator.get_macro_steps_per_sim()) {
        if (simulator.at_beginning_of_macro_step()) {
            if (simulator.bVerbose) cout << "Beginning macro step " << simulator.iCurrentMacroTimeStep + 1
                                         << " of " << simulator.get_macro_steps_per_sim() << endl;
            simulator.set_agent_turn_order();
        }

        // Loop through the micro steps
        bool bEnteredMicroStepLoop = false;
        while (!simulator.at_beginning_of_macro_step() || !bEnteredMicroStepLoop) {
            bEnteredMicroStepLoop = true;
            int iAgentIndex = simulator.get_agent_turn_order()[simulator.iCurrentMicroTimeStep % simulator.get_micro_steps_per_macro_step()];
            if (simulator.is_ai_agent(iAgentIndex)) {
                // If the agent is AI-controlled, return state observation, reward, terminated, and truncated
                vector<double> vecDbStateObservation = simulator.generate_state_observation(iAgentIndex);
                double dbReward = simulator.generate_reward(iAgentIndex);
                bool bTerminated = simulator.is_bankrupt(iAgentIndex);
                bool bTruncated = false;
                return std::make_tuple(vecDbStateObservation, dbReward, bTerminated, bTruncated);
            }
            else {
                // Perform the micro step
                simulator.perform_micro_step_control_agent_or_skip_turn(iAgentIndex);
            }
        } // End of loop over micro steps
    } // End of loop over macro steps

    // At the end of the macro steps (i.e., at the end of the current simulation). Return state observation indicating
    // the end of the current simulation.
    // TODO: Check whether this last observation really matters. Just creating an observation to make the SB3 API
    //  happy, but it's arbitrarily just using the next AI AgentID since the last observation shouldn't matter.
    vector<double> vecDbStateObservation = simulator.generate_state_observation(simulator.get_next_AI_agent_index());
    double dbReward = 0.0;
    bool bTerminated = false;
    bool bTruncated = true;
    return std::make_tuple(vecDbStateObservation, dbReward, bTerminated, bTruncated);
} // End of PythonAPI::step_helper() method

int PythonAPI::close() {
    if (simulator.bGenerateMasterOutput) {
        if (simulator.masterHistory.generate_master_output()) {
            cerr << "Error generating master output file" << endl;
            return 1;
        }
    }
    return 0;
}