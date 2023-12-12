//
// Created by Eric Thomas on 12/5/23.
//

#include "PythonAPI.h"

int PythonAPI::run_business_strategy_simulator(string strJsonConfigs) {

    // Create simulator instance
    Simulator simulator;

    // TODO: write code to check that all values in the JSON config file are valid

    if (simulator.load_json_configs(strJsonConfigs))
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
