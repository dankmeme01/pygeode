#pragma once
#include "shared.hpp"

/* Internal module (named '_internal') */

namespace module::_internal {
    PYMETHOD_DECL(call_original);
    PYMETHOD_DECL(mbo_string);
    PYMETHOD_DECL(mbo_int);
    PYMETHOD_DECL(mbo_bool);
    PYMETHOD_DECL(mbo_ptr);

    static PyMethodDef methods[] = {
        {"call_original", call_original, METH_VARARGS, "call a GD function"},
        {"mbo_string", mbo_string, METH_VARARGS, ""},
        {"mbo_int", mbo_int, METH_VARARGS, ""},
        {"mbo_bool", mbo_bool, METH_VARARGS, ""},
        {"mbo_ptr", mbo_ptr, METH_VARARGS, ""},
        {NULL, NULL, 0, NULL}
    };

    static PyModuleDef def = {
        PyModuleDef_HEAD_INIT,
        "_internal",
        "Internal pygeode module module",
        -1,
        methods
    };
}

PyMODINIT_FUNC PyInit__internal();