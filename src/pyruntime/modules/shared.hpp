#pragma once
#include <pyruntime/pyruntime.hpp>
// Code that is shared between different modules.

#define PYMETHOD_DECL(name) PyObject* name(PyObject* self, PyObject* args)