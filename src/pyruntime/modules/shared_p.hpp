#pragma once
// Code that is shared between module translation units, and should not be included in headers because of namespace pollution.

#define CHECK_ARGS(args, size) \
    do { \
        int needed = (size); \
        int argsize = PyTuple_Size((args)); \
        if (argsize != needed) { \
            PyErr_Format(PyExc_TypeError, "%s expects exactly %d arguments, but %d were provided", __func__, needed, argsize); \
            return nullptr; \
        } \
    } while (false); \

#define STRINGIFY(x) #x
#define UNPACK_PTR(args, type, out) \
    type* out = PyRuntime::get().objectToPtr<type>(PyTuple_GetItem(args, 0)).value_or(nullptr); \
    \
    if (out == nullptr) { \
        PyErr_Format(PyExc_ValueError, "Malformed " STRINGIFY(type) " instance"); \
        return nullptr; \
    }

#define CONCAT_HELPER(a, b) a ## _ ## b
#define MAKE_CALLER(class, func) \
    PyObject* CONCAT_HELPER(class, func)(PyObject* self, PyObject* args) { \
        CHECK_ARGS(args, 1); \
        UNPACK_PTR(args, class, object); \
        object->func(); \
        Py_RETURN_NONE; \
    }