//
// Created by Eric Thomas on 12/5/23.
//

#pragma once

#include "../Simulator/Simulator.h"
#include <iostream>
#include <string>

using std::cerr;
using std::cout;
using std::endl;
using std::tuple;
using std::map;

class PythonAPI {
public:
    int run_business_strategy_simulator(string strJsonConfigs);

    int init_simulator(string strJsonConfigs);

    // Returns tuple containing observation, reward, terminated, truncated
    tuple<vector<double>, double, bool, bool> step(int iActionID);

    int reset();

    // TODO: Create a close method

private:
    Simulator simulator;
    bool at_beginning_of_macro_step();
    int iCurrentMicroTimeStepWithinMacroStep = 0;
    int iCurrentMacroStepWithinSimulation = 0;
};