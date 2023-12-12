//
// Created by Eric Thomas on 12/2/23.
//

#include <pybind11/pybind11.h>
#include "../WorkingFiles/PythonAPI/PythonAPI.h"

PYBIND11_MODULE(bss_module_december_sixth, m) {
    // Expose the API class
    pybind11::class_<PythonAPI>(m, "PythonAPI")
            .def(pybind11::init<>())
            .def("run_bss2_simulator", &PythonAPI::run_business_strategy_simulator);
}