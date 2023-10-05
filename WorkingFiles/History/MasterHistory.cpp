//
// Created by Eric Thomas on 9/9/23.
//

#include "MasterHistory.h"

SimulationHistory* MasterHistory::getCurrentSimulationHistoryPtr() {
    return vecSimulationHistoryPtrs.back();
}