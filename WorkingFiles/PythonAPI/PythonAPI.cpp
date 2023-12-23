//
// Created by Eric Thomas on 12/5/23.
//

#include "PythonAPI.h"

/* TODO: Factor out a function called step() and just keep calling it repeatedly.
 * Make sure the logic for the updates to the tracking variables in in place.
 * Update main.cpp to match the work done here
 * Test to make sure this works correctly without AI agents, including correct generation of output file
*/

int PythonAPI::run_business_strategy_simulator(string strJsonConfigs) {

    // TODO: make this part of the close function, or don't include master output generation at all
//    if (simulator.masterHistory.generate_master_output()) {
//        cerr << "Error generating master output file" << endl;
//        return 1;
//    }

    return 0;
}

int PythonAPI::init_simulator(string strJsonConfigs) {
    // TODO: write code to check that all values in the JSON config file are valid

    if (simulator.load_json_configs(strJsonConfigs))
        return 1;

    if (simulator.prepare_to_run())
        return 1;

    return 0;
}

// This function assumes that init_simulator has already been called
int PythonAPI::reset() {
    if (simulator.reset())
        return 1;
    return 0;
}

// TODO: implement error checking; was using integer error codes but had to change this
// TODO: figure out how to execute the AI agent action once it has been chosen
// Returns tuple containing observation, reward, terminated, truncated
tuple<vector<double>, double, bool, bool> PythonAPI::step(int iActionID) {
    // Execute the given AI agent action
    // simulator.execute_AI_action(iActionID);

    // TODO: need to get the index of the next AI agent whose turn it is and then call the execute micro step method

    // Loop through the macro steps
    while (iCurrentMacroStepWithinSimulation < simulator.get_macro_steps_per_sim()) {
        if (at_beginning_of_macro_step()) {
            if (simulator.bVerbose) cout << "Beginning macro step " << simulator.iCurrentMacroTimeStep + 1
                                         << " of " << simulator.get_macro_steps_per_sim() << endl;
            simulator.set_agent_turn_order();
        }

        // Loop through the micro steps
        while (iCurrentMicroTimeStepWithinMacroStep < simulator.get_micro_steps_per_macro_step()) {
            int iAgentIndex = simulator.get_agent_turn_order()[simulator.iCurrentMicroTimeStep];
            if (simulator.is_ai_agent(iAgentIndex)) {
                // If the agent is AI-controlled, return state observation, reward, terminated, and truncated
                vector<double> vecDbStateObservation = simulator.generate_state_observation(iAgentIndex);
                double dbReward = simulator.generate_reward(iAgentIndex);
                bool bTerminated = simulator.is_bankrupt(iAgentIndex);
                bool bTruncated = false;
                return std::make_tuple(vecDbStateObservation, dbReward, bTerminated, bTruncated);
            }
            else {
                // Perform the micro step and increment the tracking variables
                simulator.perform_micro_step(iAgentIndex);
                iCurrentMicroTimeStepWithinMacroStep++;
            }
        }
        // At the end the loop over the micro steps. Increment the macro step and reset the micro step to 0.
        iCurrentMacroStepWithinSimulation++;
        iCurrentMicroTimeStepWithinMacroStep = 0;
    } // End of loop over macro steps

    // At the end of the macro steps (i.e., at the end of the current simulation). Return state observation indicating
    // the end of the current simulation.
    vector<double> vecDbStateObservation;
    double dbReward = 0.0;
    bool bTerminated = false;
    bool bTruncated = true;
    return std::make_tuple(vecDbStateObservation, dbReward, bTerminated, bTruncated);

} // End of PythonAPI::step() method

bool PythonAPI::at_beginning_of_macro_step() {
    return simulator.iCurrentMicroTimeStep == 0;
}
