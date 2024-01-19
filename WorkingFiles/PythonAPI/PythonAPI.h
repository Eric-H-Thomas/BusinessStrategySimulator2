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
    int init_simulator(string strJsonConfigs);
    vector<double> reset(); // Returns observation
    tuple<vector<double>, double, bool, bool> step(int iActionID); // Returns tuple containing observation, reward, terminated, truncated
    tuple<vector<double>, double, bool, bool> step_helper();
    int close();

    // Helper functions for defining the state and action spaces
    int get_num_markets();
    int get_num_agents();

private:
    Simulator simulator;
};