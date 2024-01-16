#include "geode.hpp"
#include "shared_p.hpp"
#include <hookdetours.hpp>

#define LOG_WRAP(logfn,args) \
    int _argsize = PyTuple_Size(args); \
    for (int _argi = 0; _argi < _argsize; ++_argi) { \
        PyObject* _arg = PyTuple_GetItem(args, _argi); \
        PyObject* _str_arg = PyObject_Str(_arg); \
        const char* _str = PyUnicode_AsUTF8(_str_arg); \
        logfn("{}", _str); \
        Py_XDECREF(_str_arg); \
    } \
    Py_RETURN_NONE


PyMODINIT_FUNC PyInit__geode() {
    auto& rt = PyRuntime::get();

    PyObject* module = PyModule_Create(&module::_geode::def);

    if (module == nullptr) {
        geode::log::error("failed to initialize the internal python module for geode");
        return nullptr;
    }

    // set attributes

    return module;
}

using namespace geode::prelude;

namespace module::_geode {
    PyObject* call_original(PyObject* self, PyObject* args) {
        return hookdetours::callOriginal(args);
    }

    PyObject* get_base(PyObject* self, PyObject* args) {
        return PyRuntime::get().makeInt<uintptr_t>(geode::base::get());
    }

    PyObject* get_cocos_base(PyObject* self, PyObject* args) {
        return PyRuntime::get().makeInt<uintptr_t>(geode::base::getCocos());
    }

    PyObject* log_debug(PyObject* self, PyObject* args) {
        LOG_WRAP(geode::log::debug, args);
    }

    PyObject* log_info(PyObject* self, PyObject* args) {
        LOG_WRAP(geode::log::info, args);
    }

    PyObject* log_warn(PyObject* self, PyObject* args) {
        LOG_WRAP(geode::log::warn, args);
    }

    PyObject* log_error(PyObject* self, PyObject* args) {
        LOG_WRAP(geode::log::error, args);
    }

    PyObject* mod_get(PyObject* self, PyObject* args) {
        return PyRuntime::get().makeInt<uintptr_t>((uintptr_t)geode::Mod::get());
    }

    PyObject* mod_patch(PyObject* self, PyObject* args) {
        auto& rt = PyRuntime::get();

        CHECK_ARGS(args, 3);
        UNPACK_PTR(args, geode::Mod, mod);

        uintptr_t patch;

        try {
            void* address = (void*)rt.unpackFromTuple<uintptr_t>(args, 1);
            auto data = rt.unpackFromTuple<std::vector<uint8_t>>(args, 2);
            patch = (uintptr_t)mod->patch(address, data).unwrap();
        } catch (const std::exception& e) {
            PyErr_Format(PyExc_RuntimeError, "failed to create patch: %s", e.what());
            return nullptr;
        }

        return PyRuntime::get().makeInt<uintptr_t>(patch);
    }

    PyObject* mod_add_hook(PyObject* self, PyObject* args) {
        auto& rt = PyRuntime::get();

        CHECK_ARGS(args, 3);
        UNPACK_PTR(args, geode::Mod, mod);

        uintptr_t hook;

        try {
            auto name = rt.unpackFromTuple<std::string>(args, 1);
            PyObject* userDetour = PyTuple_GetItem(args, 2);
            hook = (uintptr_t)hookdetours::addHook(mod, name, userDetour);

            Py_INCREF(userDetour);
        } catch (const std::exception& e) {
            PyErr_Format(PyExc_RuntimeError, "failed to create hook: %s", e.what());
            return nullptr;
        }

        return PyRuntime::get().makeInt<uintptr_t>(hook);
    }

    PyObject* mod_get_version(PyObject* self, PyObject* args) {
        CHECK_ARGS(args, 1);
        UNPACK_PTR(args, geode::Mod, mod);

        return PyRuntime::get().makeStr(mod->getVersion().toString());
    }

    PyObject* loader_get(PyObject* self, PyObject* args) {
        return PyRuntime::get().makeInt<uintptr_t>((uintptr_t)geode::Loader::get());
    }

    PyObject* Notification_create(PyObject* self, PyObject* args) {
        auto& rt = PyRuntime::get();
        CHECK_ARGS(args, 3);
        auto text = rt.unpackFromTuple<std::string>(args, 0);
        auto icon = rt.unpackFromTuple<int>(args, 1);
        auto time = rt.unpackFromTuple<float>(args, 2);

        auto notif = geode::Notification::create(text, (geode::NotificationIcon)icon, time);
        return rt.makeInt<uintptr_t>((uintptr_t)notif);
    }

    MAKE_CALLER(Notification, show);
}