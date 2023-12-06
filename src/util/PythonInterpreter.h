//
// Created by Midhun Darvin on 08/11/23.
//
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <string>
#include "../ComputationContext.h"
#pragma once

class PythonInterpreter {
public:
    PythonInterpreter() {
        // Initialize python
        Py_Initialize();
    }
    ~PythonInterpreter() {
        Py_Finalize();
    }

    int Execute(const std::string& filename, ComputationContext *context);
};


