//
// Created by Eric Thomas on 9/9/23.
//

#include "MasterHistory.h"
#include <iostream>
#include <fstream>

using std::string;
using std::cout;
using std::endl;

SimulationHistory* MasterHistory::getCurrentSimulationHistoryPtr() {
    return vecSimulationHistoryPtrs.back();
}

int MasterHistory::generate_master_output() {
    cout << "Generating master output file." << endl;

    // TODO: make this path dynamic rather than hard-coded
    string strMasterOutputPath = "/Users/eric/CLionProjects/BusinessStrategy2.0/OutputFiles/MasterOutput.csv";
    std::ofstream ofStreamMasterOutput;
    ofStreamMasterOutput.open(strMasterOutputPath);

    prepare_data_for_output();

    // Create header row
    ofStreamMasterOutput << ""              << ",";
    ofStreamMasterOutput << "Sim"           << ",";
    ofStreamMasterOutput << "Step"          << ",";
    ofStreamMasterOutput << "Firm"          << ",";
    ofStreamMasterOutput << "Agent Type"    << ",";
    ofStreamMasterOutput << "Market"        << ",";
    ofStreamMasterOutput << "Capital"       << ",";
    ofStreamMasterOutput << "Rev"           << ",";
    ofStreamMasterOutput << "Fix Cost"      << ",";
    ofStreamMasterOutput << "Var Cost"      << ",";
    ofStreamMasterOutput << "Entry Cost"    << ",";
    ofStreamMasterOutput << "In Market"     << ",";
    ofStreamMasterOutput << "Price"         << ",";
    ofStreamMasterOutput << "Quantity"      << ",";
    ofStreamMasterOutput << "Market Number" << "\n";

    for (int i = 0; i < vecDataRows.size(); i++) {
        auto row = vecDataRows.at(i);
        ofStreamMasterOutput << i                       << ",";
        ofStreamMasterOutput << row.iSim                << ",";
        ofStreamMasterOutput << row.iMicroTimeStep      << ",";
        ofStreamMasterOutput << row.iFirmID             << ",";
        ofStreamMasterOutput << row.strAgentType        << ",";
        ofStreamMasterOutput << row.iMarketID           << ",";
        ofStreamMasterOutput << row.dbCapital           << ",";
        ofStreamMasterOutput << row.dbRevenue           << ",";
        ofStreamMasterOutput << row.dbFixedCost         << ",";
        ofStreamMasterOutput << row.dbVarCost           << ",";
        ofStreamMasterOutput << row.dbEntryCost         << ",";
        ofStreamMasterOutput << row.bInMarket           << ",";
        ofStreamMasterOutput << row.dbPrice             << ",";
        ofStreamMasterOutput << row.dbQty               << ",";
        ofStreamMasterOutput << "Market # Placeholder"  << "\n";
    }


    // Brainstorming
    // Right now, the data is stored as a vector of individual simulation history pointers.
    // Within each simulation history, we have the following information:
//    map<int,int>                     mapAgentToFirm;
//    map<int,double>                  mapFirmStartingCapital;
//    map<int,double>                  mapMarketMaximumEntryCost;
//    vector<CapitalChange>            vecCapitalChanges;
//    vector<RevenueChange>            vecRevenueChanges;
//    vector<FixedCostChange>          vecFixedCostChanges;
//    vector<EntryCostChange>          vecEntryCostChanges;
//    vector<ProductionQuantityChange> vecProductionQtyChanges;
//    vector<MarketPresenceChange>     vecMarketPresenceChanges;


    ofStreamMasterOutput.close();

    return 0;
}

void MasterHistory::prepare_data_for_output() {
    // ORDER OF OUTPUT FILE PRECEDENCE:
    // 1. Simulation
    // 2. Firm
    // 3. Market
    // 4. Time step
    // (This precedence order means that the rows in the output file are organized such that the info for each time step
    // for a given simulation-firm-market combo is given before moving to the next market. The info for each market for
    // a given simulation-firm combo is given before moving on to the next firm. And the info for each firm is given
    // before moving to the next simulation.)

    // Instantiate all the data rows
    int iNumRows = vecSimulationHistoryPtrs.size() * iNumFirms * iNumMarkets * iMicroStepsPerSim;
    vecDataRows.resize(iNumRows);

    // TODO: factor this out into a separate function
    // Fill in the simulation number, firm ID, agent type, market ID, and time step
    for (int iSim = 0; iSim < vecSimulationHistoryPtrs.size(); iSim++) {
        for (int iFirm = 0; iFirm < iNumFirms; iFirm++) {
            for (int iMarket = 0; iMarket < iNumMarkets; iMarket++) {
                for (int iMicroTimeStep = 0; iMicroTimeStep < iMicroStepsPerSim; iMicroTimeStep++) {
                    int iRow = get_row_number(iSim, iFirm, iMarket, iMicroTimeStep);
                    vecDataRows.at(iRow).iSim = iSim;
                    vecDataRows.at(iRow).iFirmID = iFirm;
                    vecDataRows.at(iRow).strAgentType = "Placeholder for agent info; Fill this in!";
                    vecDataRows.at(iRow).iMarketID = iMarket;
                    vecDataRows.at(iRow).iMicroTimeStep = iMicroTimeStep;
                }
            }
        }
    }

    fill_in_capital_info();


    // Fill in revenue column
    // Fill in fixed cost column
    // Fill in variable cost column
    // Fill in entry cost column
    // Fill in market presence column
    // Fill in price column
    // Fill in quantity column
}

void MasterHistory::fill_in_capital_info() {
    for (int iSim = 0; iSim < vecSimulationHistoryPtrs.size(); iSim++) {
        for (int iFirm = 0; iFirm < iNumFirms; iFirm++) {
            // Get all the capital changes corresponding to the simulation-firm combo
            vector<CapitalChange> vecCapitalChanges;
            for (auto entry : vecSimulationHistoryPtrs.at(iSim)->vecCapitalChanges) {
                if (entry.iFirmID == iFirm) {
                    vecCapitalChanges.push_back(entry);
                }
            }

            // Get the stating capital amount
            double dbStartingCapital = vecSimulationHistoryPtrs.at(iSim)->mapFirmStartingCapital[iFirm];
            
            // Account for the case when capital never changed
            if (vecCapitalChanges.empty()) {
                int iStartRow = get_row_number(iSim, iFirm, 0,0);
                for (int i = 0; i < iNumMarkets * iMicroStepsPerSim; i++) {
                    vecDataRows.at(iStartRow + i).dbCapital = dbStartingCapital;
                }
            }

            int iCurrentTimeStep = 0;
            double dbCurrentCapital = dbStartingCapital;

            // For each entry in the vector of cap changes
            for (int i = 0; i < vecCapitalChanges.size(); i++) {
                auto entry = vecCapitalChanges[i];
                for (int iMarket = 0; iMarket < iNumMarkets; iMarket++) {
                    // Get the indices of the rows we want to update
                    int iStartRow = get_row_number(iSim, iFirm, iMarket, iCurrentTimeStep);
                    int iEndRow = get_row_number(iSim, iFirm, iMarket, entry.iMicroTimeStep);
                    // Update the rows with the current capital amount
                    for (int iRow = iStartRow; iRow < iEndRow; iRow++) {
                        vecDataRows.at(iRow).dbCapital = dbCurrentCapital;
                    }
                    // Update the time step and capital
                    iCurrentTimeStep = entry.iMicroTimeStep;
                    dbCurrentCapital = entry.dbNewCapitalQty;

                    // Fill in capital amounts for rows after the last capital change
                    if (i == vecCapitalChanges.size() - 1) {
                        iStartRow = get_row_number(iSim, iFirm, iMarket, iCurrentTimeStep);
                        iEndRow = get_row_number(iSim, iFirm, iMarket + 1, 0);
                        // Update the rows with the current capital amount
                        for (int iRow = iStartRow; iRow < iEndRow; iRow++) {
                            vecDataRows.at(iRow).dbCapital = dbCurrentCapital;
                        }
                    }
                } // End of loop through markets
            } // End of loop through vector of capital changes for current simulation-firm combo
        } // End of loop over firms
    } // End of loop over simulations
} // End of fill_in_capital_info()


int MasterHistory::get_row_number(int iCurrentSim, int iCurrentFirm, int iCurrentMarket, int iCurrentMicroStep) {
    int iRowsFromPastSimulations    = iCurrentSim    * iNumFirms * iNumMarkets * iMicroStepsPerSim;
    int iRowsFromPastFirms          = iCurrentFirm               * iNumMarkets * iMicroStepsPerSim;
    int iRowsFromPastMarkets        = iCurrentMarket                           * iMicroStepsPerSim;
    return iRowsFromPastSimulations + iRowsFromPastFirms + iRowsFromPastMarkets + iCurrentMicroStep;
}