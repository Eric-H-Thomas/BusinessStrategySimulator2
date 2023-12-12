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
};