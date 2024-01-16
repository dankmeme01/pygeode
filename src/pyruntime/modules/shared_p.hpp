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

template <typename... Args>
inline constexpr size_t arg_va_count() {
    return sizeof...(Args);
}

// non-static non-void
#define MAKE_CALLER_NSNV(class, func, rettype, ...) \
    PyObject* CONCAT_HELPER(class, func)(PyObject* self, PyObject* args) { \
        constexpr size_t argcount = arg_va_count<__VA_ARGS__>() + 1; \
        CHECK_ARGS(args, argcount); \
        UNPACK_PTR(args, class, object); \
        auto methargs = PyRuntime::get().unpackTuple<__VA_ARGS__>(args, 1); \
        using Fptr = rettype (class::*)(__VA_ARGS__); \
        Fptr fp = &class::func; \
        rettype ret = std::apply(std::mem_fn(fp), std::tuple_cat(std::tie(object), methargs)); \
        return PyRuntime::get().makePyObject(std::move(ret)); \
    }

// non-static non-void const-qualified
#define MAKE_CALLER_NSNVCQ(class, func, rettype, ...) \
    PyObject* CONCAT_HELPER(class, func)(PyObject* self, PyObject* args) { \
        constexpr size_t argcount = arg_va_count<__VA_ARGS__>() + 1; \
        CHECK_ARGS(args, argcount); \
        UNPACK_PTR(args, class, object); \
        auto methargs = PyRuntime::get().unpackTuple<__VA_ARGS__>(args, 1); \
        using Fptr = rettype (class::*)(__VA_ARGS__) const; \
        Fptr fp = &class::func; \
        rettype ret = std::apply(std::mem_fn(fp), std::tuple_cat(std::tie(object), methargs)); \
        return PyRuntime::get().makePyObject(std::move(ret)); \
    }

// non-static void
#define MAKE_CALLER_NSV(class, func, ...) \
    PyObject* CONCAT_HELPER(class, func)(PyObject* self, PyObject* args) { \
        constexpr size_t argcount = arg_va_count<__VA_ARGS__>() + 1; \
        CHECK_ARGS(args, argcount); \
        UNPACK_PTR(args, class, object); \
        auto methargs = PyRuntime::get().unpackTuple<__VA_ARGS__>(args, 1); \
        using Fptr = void (class::*)(__VA_ARGS__); \
        Fptr fp = &class::func; \
        std::apply(std::mem_fn(fp), std::tuple_cat(std::tie(object), methargs)); \
        Py_RETURN_NONE; \
    }