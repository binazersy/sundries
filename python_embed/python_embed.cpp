#include <iostream>
#include <sstream>
#include <Python.h>


int main()
{
	Py_Initialize();
	if (!Py_IsInitialized())
	{
		std::cerr << "Python initialization failed!";
		return -1;
	}

	std::istringstream iss;
	std::string in;

	// run simple string command
	PyRun_SimpleString("print(1)");

	std::cout << "Enter script file: ";
	std::getline(std::cin, in);

	PyObject *sys = PyImport_ImportModule("sys");
	PyObject *path = PyObject_GetAttrString(sys, "path");
	std::cout << PyList_Size(path) << std::endl;
	Py_DecRef(path);
	Py_DecRef(sys);


	// module import
	PyObject* py_module = PyImport_ImportModule(in.c_str());
	if (nullptr == py_module)
	{
		std::cerr << "Module import failed!" << std::endl;
		return -1;
	}

	// get function
	std::cout << "Enter function name: ";
	std::getline(std::cin, in);
	PyObject* py_fun = PyObject_GetAttrString(py_module, in.c_str());
	if (nullptr == py_fun)
	{
		std::cerr << "Func. import failed!" << std::endl;
		return -1;
	}

	if (PyCallable_Check(py_fun))
	{
		// arg
		std::cout << "Enter arg. value: ";
		std::getline(std::cin, in);
		PyObject* py_arg = PyTuple_New(1);
		PyObject* py_val = PyFloat_FromDouble(atof(in.c_str()));
		PyTuple_SetItem(py_arg, 0, py_val);

		// call
		py_val = PyObject_CallObject(py_fun, py_arg);
		if (NULL != py_arg)
		{
			// decrease ref. count
			Py_DECREF(py_arg);
		}

		std::cout << "Result: " << PyFloat_AsDouble(py_val) << std::endl;
	}
	else
	{
		PyErr_Print();
	}

	if (NULL != py_module)
		Py_DECREF(py_module);

	Py_Finalize();

	return 0;
}