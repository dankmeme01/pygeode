#pragma once
#include <Geode/Geode.hpp>
#include <pyruntime/pyruntime.hpp>

#define COMMON_UD(ud) \
        common(ud); \
        storedDetour = ud; \

#define CHECK_HOOK(expected, address, convention, func) \
    if (std::string_view(expected) == (name)) \
        return mod->addHook((void*)(geode::base::get() + address), fnvars::func(userDetour), std::string(name), convention).unwrap(); \


namespace hookdetours {
    template <typename T>
    struct StoredDetour {
        void* address;
        int origPlacement;

    };

    namespace fnvars {
        inline void common(PyObject* userDetour) {
            if (!PyCallable_Check(userDetour)) {
                throw std::runtime_error("invalid callable user detour passed");
            }
        }
    }

    [[noreturn]] inline void hookFail(const std::string_view func) {
        throw std::runtime_error(std::string("attempted to hook a function with no binding: ") + std::string(func));
    }

    inline void handleException(PyObject* retval) {
        if (!retval) {
            PyRuntime::get().throwPyError();
        }

        if (!Py_IsNone(retval)) {
            Py_DECREF(retval);
        }
    }

// non-static non-void
#define CALL_ORIG_HOOK_NSNV(_name, cls, rettype, func, ...) \
    if (name == _name) { \
        auto* self = rt.objectToPtr<cls>(PyTuple_GetItem(args, 1)).value_or(nullptr); \
        if (!self) { \
            PyErr_Format(PyExc_ValueError, "Malformed " #cls " instance"); \
            return nullptr; \
        } \
        auto methargs = rt.unpackTuple<__VA_ARGS__>(args, 2); \
        using Fptr = rettype (cls::*)(__VA_ARGS__); \
        Fptr fp = &cls::func; \
        rettype ret = std::apply(std::mem_fn(fp), std::tuple_cat(std::tie(self), methargs)); \
        return rt.makePyObject(std::move(ret)); \
    } \

// non-static void
#define CALL_ORIG_HOOK_NSV(_name, cls, func, ...) \
    if (name == _name) { \
        auto* self = rt.objectToPtr<cls>(PyTuple_GetItem(args, 1)).value_or(nullptr); \
        if (!self) { \
            PyErr_Format(PyExc_ValueError, "Malformed " #cls " instance"); \
            return nullptr; \
        } \
        auto methargs = rt.unpackTuple<__VA_ARGS__>(args, 2); \
        using Fptr = void (cls::*)(__VA_ARGS__); \
        Fptr fp = &cls::func; \
        std::apply(std::mem_fn(fp), std::tuple_cat(std::tie(self), methargs)); \
        Py_RETURN_NONE; \
    } \

// static non-void TODO
// static void TODO

    PyObject* callOriginal(PyObject* args);

};