#include "Simulator/Simulator.h"
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;

// TODO: For testing; comment out when done
//#include "PythonAPI/PythonAPI.h"
//int main(int argc, char* argv[]) {
//    string strJsonconfigs = argv[1];
//    run_business_strategy_simulator(strJsonconfigs);
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

    for (int iSim = 0; iSim < simulator.getNumSims(); iSim++) {
        cout << "Beginning simulation " << iSim << " of " << simulator.getNumSims()-1 << " (indexed at 0)" << endl;
        if (simulator.run())
            return 1;

        // Reset the simulator unless it is the last time running it
        if (iSim != simulator.getNumSims() - 1) {
            if (simulator.reset())
                return 1;
        }
    }

    if (simulator.masterHistory.generate_master_output()) {
        cerr << "Error generating master output file" << endl;
        return 1;
    }

    return 0;
}
