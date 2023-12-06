//
// Created by Midhun Darvin on 08/11/23.
//

#include "PythonInterpreter.h"

std::string GetStringFromDict(PyObject *dict, const std::string& key)
{
    // Get the value associated with the specified key
    PyObject *value = PyDict_GetItemString(dict, key.c_str());
    if (value)
    {
        // Check if the returned value is a string
        if (PyUnicode_Check(value))
        {
            // Convert the Python string to a C string
            const char *c_value = PyUnicode_AsUTF8(value);

            // Print the result
            std::cout << "Value for key '" << key << "': " << c_value << std::endl;
            return c_value;
        }
        else {
            std::cerr << "The value is not a string." << std::endl;
        }
    } else {
        std::cerr << "Key '" << key << "' not found in the dictionary." << std::endl;
    }
    return "";
}

PyObject *createExecutionContext(ComputationContext *context)
{
    // Create a Python dictionary
    PyObject *dict = PyDict_New();
    if (!dict)
    {
        PyErr_Print();
        return nullptr;
    }

    auto params = std::any_cast<std::unordered_map<std::string, std::string>>(context->Get("params"));

    for (const auto &param : params)
    {
        PyObject *strA = PyUnicode_FromString(param.first.c_str());
        PyObject *strB = PyUnicode_FromString(param.second.c_str());
        PyDict_SetItem(dict, strA, strB);
        Py_XDECREF(strA);
        Py_XDECREF(strB);
    }

    // Print the dictionary
    PyObject_Print(dict, stdout, 0);
    printf("\n");
    return dict;
}

int PythonInterpreter::Execute(const std::string& filename, ComputationContext *context)
{

    if (!std::getenv("PLUGINS_FOLDER_PATH")) {
        throw std::runtime_error("PLUGINS_FOLDER_PATH environment variable is missing!");
    }
    std::string pythonPluginsFolderPath = std::string(std::getenv("PLUGINS_FOLDER_PATH")) + "/python-handlers";

    PyObject *pName, *pModule;
    PyObject *preExecuteFunc, *executeFunc, *postExecuteFunc;

    PyRun_SimpleString("import sys");
    std::string addPluginsPath = "sys.path.append(\"" + pythonPluginsFolderPath + "\")";
    PyRun_SimpleString(addPluginsPath.c_str());

    pName = PyUnicode_DecodeFSDefault(filename.c_str());
    /* Error checking of pName left out */

    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL)
    {

        preExecuteFunc = PyObject_GetAttrString(pModule, "preExecute");
        executeFunc = PyObject_GetAttrString(pModule, "execute");
        postExecuteFunc = PyObject_GetAttrString(pModule, "postExecute");

        PyObject *executionContext = createExecutionContext(context);
        PyObject *pArgs = PyTuple_New(1);
        PyTuple_SetItem(pArgs, 0, executionContext);
        if (preExecuteFunc && PyCallable_Check(preExecuteFunc))
        {
            PyObject *preExecuteResult = PyObject_CallObject(preExecuteFunc, pArgs);
            if (preExecuteResult != NULL)
            {
                std::cout << "PreExecute :" << Py_IsTrue(preExecuteResult) << std::endl;
                if (Py_IsTrue(preExecuteResult))
                {
                    if (executeFunc && PyCallable_Check(executeFunc))
                    {
                        PyObject *executeFuncResult = PyObject_CallObject(executeFunc, pArgs);
                        std::cout << "Execute :" << Py_IsTrue(executeFuncResult) << std::endl;
                        std::string message = "Execution status : " + std::string((Py_IsTrue(executeFuncResult) ? "true" : "false"));
                        context->Put("message", message);
                        std::string result = GetStringFromDict(executionContext, "response");
                        context->Put("response", result);
                        if (executeFuncResult)
                        {
                            Py_DECREF(executeFuncResult);
                        }
                    }
                    else
                    {
                        if (PyErr_Occurred())
                            PyErr_Print();
                        fprintf(stderr, "Cannot find function \"%s\"\n", "preExecute");
                        Py_DECREF(executeFunc);
                        Py_DECREF(pModule);
                    }

                    if (postExecuteFunc && PyCallable_Check(postExecuteFunc))
                    {
                        PyObject *postExecuteFuncResult = PyObject_CallObject(postExecuteFunc, pArgs);
                        std::cout << "PostExecute :" << Py_IsTrue(postExecuteFuncResult) << std::endl;
                        Py_DECREF(postExecuteFuncResult);
                    }
                    else
                    {
                        if (PyErr_Occurred())
                            PyErr_Print();
                        fprintf(stderr, "Cannot find function \"%s\"\n", "preExecute");
                        Py_DECREF(postExecuteFunc);
                        Py_DECREF(pModule);
                    }
                }
                Py_DECREF(preExecuteResult);
            }
        }
        else
        {
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", "preExecute");
        }

        Py_DECREF(pModule);
    }
    else
    {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", filename.c_str());
    }

    if (Py_FinalizeEx() < 0)
    {
        return 1;
        //        exit(120);
    }
    //    PyMem_RawFree(program);
}
