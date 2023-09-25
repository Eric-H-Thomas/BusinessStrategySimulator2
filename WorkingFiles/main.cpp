#include "Simulator/Simulator.h"
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;

int main(int argc, char* argv[]) {

    // Check for correct number of command-line arguments
    if (argc != 2) {
        cerr << "Expected 1 command-line argument. Got " << argc-1 << endl;
        return 1;
    }

    // Create simulator instance
    Simulator simulator = Simulator();

    // TODO: write code to check that all values in the JSON config file are valid
    // Load the JSONReader configs for the simulator
    if (simulator.load_json_configs(argv[1]))
        return 1;

    // TODO: within the run preparation, include code that accounts for the various levels of randomization
    if (simulator.prepare_to_run())
        return 1;

    // TODO: within the run method, include code that accounts for the various levels of randomization
    for (int iSim = 0; iSim < simulator.getNumSims(); iSim++){
        if (simulator.run())
            return 1;
    }

    // TODO: Save the results to an output file

    return 0;
}
