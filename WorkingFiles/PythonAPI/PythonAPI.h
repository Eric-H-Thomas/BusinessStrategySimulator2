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

class PythonAPI {
public:
    int run_business_strategy_simulator(string strJsonConfigs);

    // TODO: Create an initialization method
    int init_simulator(string strJsonConfigs);

    // TODO: Create a step method

    // TODO: Create a reset method

    // TODO: Create a close method

private:
    Simulator simulator;
};