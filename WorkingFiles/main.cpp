/*
This main function is used to run the simulator on its own, with control and/or pre-trained AI agents.
Use business_strategy_gym_env.py or another Python script for training AI agents.
*/

#include "Simulator/Simulator.h"
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;

// For debugging the Python API
//#include "PythonAPI/PythonAPI.h"
//int main(int argc, char* argv[]) {
//
//    // Check for correct number of command-line arguments
//    if (argc != 2) {
//        cerr << "Expected 1 command-line argument. Got " << argc-1 << endl;
//        return 1;
//    }
//
//    // Create PythonAPI instance
//    PythonAPI api;
//
//    // Test the API init method
//    if (api.init_simulator(argv[1]))
//        return 1;
//
//    for (int iTest = 0; iTest < 100; iTest++) {
//        // Test the API reset method
//        //cout << "Debugging: About to run the reset method" << endl;
//        auto reset_return_values = api.reset();
//
//        // Test the API step method, instructing the AI to flip its presence in market 0
//        //cout << "Debugging: About to run the step method" << endl;
//        auto step_return_values = api.step(0);
//    }
//
//    // Test the API close method
//    if (api.close())
//        return 1;
//
//    return 0;
//}

int main(int argc, char* argv[]) {

    // Check for correct number of command-line arguments
    if (argc != 2) {
        cerr << "Expected 1 command-line argument. Got " << argc-1 << endl;
        return 1;
    }

    // Create simulator instance
    Simulator simulator;

    // TODO: write code to check that all values in the JSON config file are valid

    if (simulator.load_json_configs(argv[1]))
        return 1;

    if (simulator.prepare_to_run())
        return 1;

    for (int iSim = 0; iSim < simulator.get_num_sims(); iSim++) {
        cout << "Beginning simulation " << iSim << " of " << simulator.get_num_sims() - 1 << " (indexed at 0)" << endl;
        if (simulator.reset())
            return 1;

        if (simulator.run())
            return 1;
    }

    if (simulator.bGenerateMasterOutput) {
        if (simulator.masterHistory.generate_master_output()) {
            cerr << "Error generating master output file" << endl;
            return 1;
        }
    }

    return 0;
}
