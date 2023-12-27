#cmake_minimum_required(VERSION 3.20)
#project(BusinessStrategy2_0)
#
#set(CMAKE_CXX_STANDARD 14)
#
#add_subdirectory(external/pybind11)
#include_directories(external/pybind11/include)
#
#add_executable(BusinessStrategy2_0 WorkingFiles/main.cpp WorkingFiles/Simulator/Simulator.cpp WorkingFiles/Simulator/Simulator.h WorkingFiles/Agent/ControlAgent.cpp WorkingFiles/Agent/ControlAgent.h WorkingFiles/Economy/Economy.cpp WorkingFiles/Economy/Economy.h JSONReader/json.h WorkingFiles/Firm/Firm.cpp WorkingFiles/Firm/Firm.h WorkingFiles/Market/Market.cpp WorkingFiles/Market/Market.h WorkingFiles/History/SimulationHistory.cpp WorkingFiles/History/SimulationHistory.h WorkingFiles/History/MasterHistory.cpp WorkingFiles/History/MasterHistory.h WorkingFiles/Utils/MiscUtils.cpp WorkingFiles/Utils/MiscUtils.h WorkingFiles/Action/Action.cpp WorkingFiles/Action/Action.h WorkingFiles/Utils/StringUtils.cpp WorkingFiles/Utils/StringUtils.h WorkingFiles/DataCache/DataCache.cpp WorkingFiles/DataCache/DataCache.h WorkingFiles/PyBindTest/PyBindTest.cpp WorkingFiles/PyBindTest/PyBindTest.h WorkingFiles/python_wrapper.cpp WorkingFiles/PythonAPI/PythonAPI.cpp WorkingFiles/PythonAPI/PythonAPI.h)
#target_link_libraries(BusinessStrategy2_0 PRIVATE pybind11::embed)
#
#pybind11_add_module(bss_module_december_sixth WorkingFiles/python_wrapper.cpp WorkingFiles/PythonAPI/PythonAPI.cpp)

cmake_minimum_required(VERSION 3.20)
project(BusinessStrategy2_0)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(external/pybind11)
include_directories(external/pybind11/include)

# List all the source files used by both the executable and the Pybind11 module
set(COMMON_SOURCES
        WorkingFiles/main.cpp
        WorkingFiles/Simulator/Simulator.cpp
        WorkingFiles/Simulator/Simulator.h
        WorkingFiles/Agent/ControlAgent.cpp
        WorkingFiles/Agent/ControlAgent.h
        WorkingFiles/Economy/Economy.cpp
        WorkingFiles/Economy/Economy.h
        JSONReader/json.h
        WorkingFiles/Firm/Firm.cpp
        WorkingFiles/Firm/Firm.h
        WorkingFiles/Market/Market.cpp
        WorkingFiles/Market/Market.h
        WorkingFiles/History/SimulationHistory.cpp
        WorkingFiles/History/SimulationHistory.h
        WorkingFiles/History/MasterHistory.cpp
        WorkingFiles/History/MasterHistory.h
        WorkingFiles/Utils/MiscUtils.cpp
        WorkingFiles/Utils/MiscUtils.h
        WorkingFiles/Action/Action.cpp
        WorkingFiles/Action/Action.h
        WorkingFiles/Utils/StringUtils.cpp
        WorkingFiles/Utils/StringUtils.h
        WorkingFiles/DataCache/DataCache.cpp
        WorkingFiles/DataCache/DataCache.h
        WorkingFiles/PyBindTest/PyBindTest.cpp
        WorkingFiles/PyBindTest/PyBindTest.h
        WorkingFiles/python_wrapper.cpp
        WorkingFiles/PythonAPI/PythonAPI.cpp
        WorkingFiles/PythonAPI/PythonAPI.h
        WorkingFiles/Agent/BaseAgent.cpp WorkingFiles/Agent/BaseAgent.h WorkingFiles/Agent/StableBaselines3Agent.cpp WorkingFiles/Agent/StableBaselines3Agent.h)

# Executable target
add_executable(BusinessStrategy2_0 WorkingFiles/main.cpp ${COMMON_SOURCES})
target_link_libraries(BusinessStrategy2_0 PRIVATE pybind11::embed)

# Pybind11 module target
pybind11_add_module(bss_module_december_twentysixth ${COMMON_SOURCES})