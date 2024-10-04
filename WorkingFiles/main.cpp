/*
This main function is used to run the simulator on its own, with control and/or pre-trained AI agents.
Use business_strategy_gym_env.py or another Python script for training AI agents.
*/

#define PY_SSIZE_T_CLEAN

#include <pybind11/embed.h> // everything needed for embedding
#include <Python.h>
#include <iostream>
#include "Simulator/Simulator.h"
#include <pybind11/pybind11.h>

using std::cerr;
using std::cout;
using std::endl;

namespace py = pybind11;

int main(int argc, char* argv[]) {

    // Check for correct number of command-line arguments
    if (argc != 2) {
        cerr << "Expected 1 command-line argument. Got " << argc - 1 << endl;
        return 1;
    }

    py::scoped_interpreter guard{}; // start the interpreter and keep it alive
    py::object simulate_function;
    try {
        // Add the directory containing the script to the Python path
        py::module::import("sys").attr("path").attr("append")("/Users/eric/CLionProjects/BusinessStrategy2.0/");

        // Import the script
        py::module script = py::module::import("simulator");

        // Set the simulate function py::object equal to the simulator.py simulate function
        simulate_function = script.attr("simulate");
    }
    catch (const py::error_already_set &e) {
        std::cerr << "Error: " << e.what() << std::endl;
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

        if (simulator.run(simulate_function))
            return 1;
    }

    if (simulator.bGenerateMasterOutput) {
        if (simulator.masterHistory.generate_master_output()) {
            cerr << "Error generating master output file" << endl;
            return 1;
        }
        if (simulator.masterHistory.generate_market_overlap_file()) {
            cerr << "Error generating market overlap file" << endl;
            return 1;
        }
    }

    return 0;
}