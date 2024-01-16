#pragma once
#define PY_SSIZE_T_CLEAN

#include <Python.h>

#define MAKE_C_STRING(sv, _strPtr) \
    char _strStackBuf[512]; \
    char* _strPtr; \
    if (sv.size() < 512) { \
        _strPtr = _strStackBuf; \
    } else { \
        _strPtr = new char[sv.size() + 1]; \
    } \
    strncpy(_strPtr, sv.data(), sv.size()); \
    _strPtr[sv.size()] = '\0'; \

#define FREE_C_STRING(strPtr) \
    if (strPtr != _strStackBuf) { \
        delete[] strPtr; \
    } \

class PyRuntime {
    PyRuntime();
    ~PyRuntime();

public:
    PyRuntime(const PyRuntime& other) = delete;
    PyRuntime& operator=(const PyRuntime& other) = delete;
    PyRuntime(PyRuntime&& other) = delete;
    PyRuntime&& operator=(PyRuntime&& other) = delete;

    static PyRuntime& get() {
        static PyRuntime runtime;
        return runtime;
    }

    void appendPath(const std::string_view path);

    void throwPyError();

    void runSimpleString(const std::string_view code);
    void runSimpleString(const char* code);

    void runString(const std::string_view code);
    void runString(const char* code);

    void runStringFromFile(const ghc::filesystem::path path);

    template <typename T>
    T unpackFromTuple(PyObject* tuple, Py_ssize_t index) {
        PyObject* item = PyTuple_GetItem(tuple, index);
        if (!item) {
            throw std::runtime_error("failed to unpack value from tuple");
        }

        return fromPyObject<T>(item);
    }

    template <typename... Args, size_t... Indices>
    std::tuple<Args...> unpackTupleHelper(PyObject* tuple, Py_ssize_t startIndex, std::index_sequence<Indices...>) {
        return std::make_tuple(unpackFromTuple<std::tuple_element_t<Indices, std::tuple<Args...>>>(tuple, startIndex + Indices)...);
    }

    template <typename... Args>
    std::tuple<Args...> unpackTuple(PyObject* tuple, Py_ssize_t startIndex) {
        return unpackTupleHelper<Args...>(tuple, startIndex, std::index_sequence_for<Args...>());
    }

    template <typename T>
    PyObject* makePyObject(T&& val) {
        if constexpr (std::is_pointer_v<T>) {
            return makeInt<uintptr_t>((uintptr_t)val);
        } else if constexpr (std::is_same_v<T, bool>) {
            return makeBool(val);
        } else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
            return makeInt(val);
        } else if constexpr (std::is_same_v<T, std::string_view>) {
            MAKE_C_STRING(val, string);
            auto ret = makeStr(string);
            FREE_C_STRING(string);
            return ret;
        } else if constexpr (std::is_same_v<T, gd::string> || std::is_same_v<T, std::string>) {
            return this->makePyObject(std::string_view(val));
        } else {
            static_assert(std::is_same_v<T, void>, "invalid type passed into makePyObject");
        }
    }

    template <typename T>
    T fromPyObject(PyObject* item) {
        if constexpr (std::is_pointer_v<T>) {
            if (!PyLong_Check(item)) {
                throw std::runtime_error("invalid argument type was passed");
            }

            T num = reinterpret_cast<T>(static_cast<uintptr_t>(PyLong_AsUnsignedLongLong(item)));
            return num;
        } else if constexpr (std::is_integral_v<T>) {
            if (!PyLong_Check(item)) {
                throw std::runtime_error("invalid argument type was passed");
            }

            T num;
            if constexpr (std::is_unsigned_v<T>) {
                num = static_cast<T>(PyLong_AsUnsignedLongLong(item));
            } else {
                num = static_cast<T>(PyLong_AsLongLong(item));
            }

            return num;
        } else if constexpr (std::is_floating_point_v<T>) {
            if (!PyFloat_Check(item)) {
                throw std::runtime_error("invalid argument type was passed");
            }

            T num = static_cast<T>(PyFloat_AsDouble(item));
            return num;
        } else if constexpr (std::is_same_v<T, std::string>) {
            PyObject* string = PyObject_Str(item);
            const char* cstr = PyUnicode_AsUTF8(string);
            std::string retval(cstr);

            Py_DECREF(string);
            return retval;
        } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
            if (!PyBytes_Check(item)) {
                throw std::runtime_error("failed to unpack value from tuple, expected a 'bytes' object");
            }

            Py_ssize_t size = PyBytes_Size(item);
            char* data = PyBytes_AsString(item);

            std::vector<uint8_t> result(data, data + size);
            return result;
        } else {
            static_assert(std::is_same_v<T, void>, "invalid type passed into fromPyObject<T>");
        }
    }

    template <typename T>
    void packItemIntoTuple(PyObject* tuple, Py_ssize_t& index, T&& val) {
        PyTuple_SetItem(tuple, index++, makePyObject(std::forward<T>(val)));
    }

    template <typename... Args>
    PyObject* packTuple(Args&&... args) {
        PyObject* tuple = PyTuple_New(sizeof...(args));

        Py_ssize_t index = 0;
        (packItemIntoTuple(tuple, index, std::forward<std::remove_reference_t<Args>>(args)), ...);

        return tuple;
    }

    template <typename T>
    PyObject* makeInt(T val);

    template <typename T>
    PyObject* makeFloat(T val);

    PyObject* makeBool(bool val);

    PyObject* makeStr(const std::string_view val);
    PyObject* makeStr(const char* val);

    template <typename T>
    std::optional<T*> objectToPtr(PyObject* obj) {
        if (!PyLong_Check(obj)) return std::nullopt;

        long long val = PyLong_AsLongLong(obj);
        if (val == -1 && PyErr_Occurred()) {
            return std::nullopt;
        }

        return reinterpret_cast<T*>(static_cast<uintptr_t>(val));
    }
};
