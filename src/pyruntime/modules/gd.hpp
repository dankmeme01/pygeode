#pragma once
#include "shared.hpp"

namespace module::_gd {
    PYMETHOD_DECL(CCObject_retain);
    PYMETHOD_DECL(CCObject_release);

    PYMETHOD_DECL(FLAlertLayer_create);
    PYMETHOD_DECL(FLAlertLayer_show);

    static PyMethodDef methods[] = {
        {"CCObject_retain", CCObject_retain, METH_VARARGS, ""},
        {"CCObject_release", CCObject_release, METH_VARARGS, ""},

        {"FLAlertLayer_create", FLAlertLayer_create, METH_VARARGS, ""},
        {"FLAlertLayer_show", FLAlertLayer_show, METH_VARARGS, ""},
        {NULL, NULL, 0, NULL}
    };

    static PyModuleDef def = {
        PyModuleDef_HEAD_INIT,
        "_gd",
        "GD and cocos functions",
        -1,
        methods
    };
}

PyMODINIT_FUNC PyInit__gd();