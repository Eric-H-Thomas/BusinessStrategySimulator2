cmake_minimum_required(VERSION 3.20)
project(BusinessStrategy2_0)

set(CMAKE_CXX_STANDARD 14)

add_executable(BusinessStrategy2_0 WorkingFiles/main.cpp WorkingFiles/Simulator/Simulator.cpp WorkingFiles/Simulator/Simulator.h WorkingFiles/Agent/ControlAgent.cpp WorkingFiles/Agent/ControlAgent.h WorkingFiles/Economy/Economy.cpp WorkingFiles/Economy/Economy.h JSONReader/json.h WorkingFiles/Firm/Firm.cpp WorkingFiles/Firm/Firm.h WorkingFiles/Market/Market.cpp WorkingFiles/Market/Market.h WorkingFiles/History/SimulationHistory.cpp WorkingFiles/History/SimulationHistory.h WorkingFiles/History/MasterHistory.cpp WorkingFiles/History/MasterHistory.h WorkingFiles/Utils/MathUtils.cpp WorkingFiles/Utils/MathUtils.h WorkingFiles/Action/Action.cpp WorkingFiles/Action/Action.h WorkingFiles/Utils/StringUtils.cpp WorkingFiles/Utils/StringUtils.h)
