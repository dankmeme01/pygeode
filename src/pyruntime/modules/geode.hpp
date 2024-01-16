#pragma once
#include "shared.hpp"

/* Geode bindings module (named '_geode') */

namespace module::_geode {
    PYMETHOD_DECL(get_base);
    PYMETHOD_DECL(get_cocos_base);

    PYMETHOD_DECL(log_debug);
    PYMETHOD_DECL(log_info);
    PYMETHOD_DECL(log_warn);
    PYMETHOD_DECL(log_error);

    PYMETHOD_DECL(mod_get);
    PYMETHOD_DECL(mod_patch);
    PYMETHOD_DECL(mod_add_hook);
    PYMETHOD_DECL(mod_get_version);

    PYMETHOD_DECL(loader_get);

    PYMETHOD_DECL(Notification_create);
    PYMETHOD_DECL(Notification_show);

    static PyMethodDef methods[] = {
        {"get_base", get_base, METH_NOARGS, "get the base address of the gd executable"},
        {"get_cocos_base", get_cocos_base, METH_NOARGS, "get the base address of cocos dll"},

        {"log_debug", log_debug, METH_VARARGS, "log a message"},
        {"log_info", log_info, METH_VARARGS, "log a message"},
        {"log_warn", log_warn, METH_VARARGS, "log a message"},
        {"log_error", log_error, METH_VARARGS, "log a message"},

        {"mod_get", mod_get, METH_NOARGS, "get an instance of the current mod"},
        {"mod_patch", mod_patch, METH_VARARGS, "patch memory"},
        {"mod_get_version", mod_get_version, METH_VARARGS, "get the version of a mod"},
        {"mod_add_hook", mod_add_hook, METH_VARARGS, "add a hook"},

        {"loader_get", loader_get, METH_NOARGS, "get an instance the loader"},

        {"Notification_create", Notification_create, METH_VARARGS, "create a new notification"},
        {"Notification_show", Notification_show, METH_VARARGS, "show the notification"},
        {NULL, NULL, 0, NULL}
    };

    static PyModuleDef def = {
        PyModuleDef_HEAD_INIT,
        "_geode",
        "Geode bindings module",
        -1,
        methods
    };
}

PyMODINIT_FUNC PyInit__geode();