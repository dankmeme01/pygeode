#include "internal.hpp"
#include "shared_p.hpp"
#include <hookdetours.hpp>

PyMODINIT_FUNC PyInit__internal() {
    auto& rt = PyRuntime::get();

    PyObject* module = PyModule_Create(&module::_internal::def);

    if (module == nullptr) {
        geode::log::error("failed to initialize the internal python module");
        return nullptr;
    }

    // set attributes

    return module;
}

using namespace geode::prelude;

#define MBO_ACCESSOR(type, object, offset) \
    CHECK_ARGS(args, 2); \
    UNPACK_PTR(args, void, _mobj); \
    auto& rt = PyRuntime::get(); \
    size_t offset = rt.unpackFromTuple<int>(args, 1); \
    type* object = reinterpret_cast<type*>((uintptr_t)_mobj + offset); \

namespace module::_internal {
    PyObject* call_original(PyObject* self, PyObject* args) {
        return hookdetours::callOriginal(args);
    }

    PyObject* mbo_string(PyObject* self, PyObject* args) {
        MBO_ACCESSOR(gd::string, object, offset);
        return rt.makeStr(*object);
    }

    PyObject* mbo_int(PyObject* self, PyObject* args) {
        MBO_ACCESSOR(int, object, offset);
        return rt.makeInt(*object);
    }

    PyObject* mbo_bool(PyObject* self, PyObject* args) {
        MBO_ACCESSOR(bool, object, offset);
        return rt.makeBool(*object);
    }

    PyObject* mbo_ptr(PyObject* self, PyObject* args) {
        MBO_ACCESSOR(void*, object, offset);
        return rt.makeInt((uintptr_t)*object);
    }
}