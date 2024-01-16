#include "pyruntime.hpp"

PyRuntime::PyRuntime() {
    Py_Initialize();

    if (!Py_IsInitialized()) {
        throw std::runtime_error("failed to initialize the python interpreter");
    }
}

PyRuntime::~PyRuntime() {
    Py_Finalize();
}

void PyRuntime::appendPath(const std::string_view path) {
    PyObject* sysModule = PyImport_ImportModule("sys");

    if (!sysModule) {
        throw std::runtime_error("failed to import the 'sys' module");
    }

    PyObject* sysPath = PyObject_GetAttrString(sysModule, "path");
    if (!PyList_Check(sysPath)) {
        throw std::runtime_error("`sys.path` is not a list");
    }

    MAKE_C_STRING(path, string);
    PyList_Append(sysPath, PyUnicode_DecodeFSDefault(string));
    FREE_C_STRING(string);
}

void PyRuntime::throwPyError() {
    if (!PyErr_Occurred()) return;

    PyObject *pType, *pValue, *pTraceback;
    PyErr_Fetch(&pType, &pValue, &pTraceback);
    PyErr_NormalizeException(&pType, &pValue, &pTraceback);

    if (!pValue) {
        throw std::runtime_error("unknown python error");
    }

    PyObject *pStr = PyObject_Str(pValue);
    const char* errorStr = PyUnicode_AsUTF8(pStr);
    geode::log::warn("Python exception: {}", errorStr);

    auto err = std::runtime_error(errorStr);

    PyErr_Clear();
    Py_DECREF(pStr);

    throw err;
}

void PyRuntime::runSimpleString(const std::string_view code) {
    MAKE_C_STRING(code, string);
    this->runSimpleString(string);
    FREE_C_STRING(string);
}

void PyRuntime::runSimpleString(const char* code) {
    PyRun_SimpleString(code);
}

void PyRuntime::runString(const std::string_view code) {
    MAKE_C_STRING(code, string);
    this->runString(string);
    FREE_C_STRING(string);
}

void PyRuntime::runString(const char* code) {
    auto mainModule = PyImport_AddModule("__main__");

    Py_INCREF(mainModule);
    auto dict = PyModule_GetDict(mainModule);
    auto result = PyRun_String(
        code, Py_file_input, dict, dict
    );

    Py_DECREF(mainModule);

    if (!result) this->throwPyError();

    Py_DECREF(result);
}

void PyRuntime::runStringFromFile(const ghc::filesystem::path path) {
    std::ifstream ifile(path);
    std::stringstream buf;
    buf << ifile.rdbuf();

    auto bufstr = buf.str();

    this->runString(bufstr);
}

template <typename T>
PyObject* PyRuntime::makeInt(T val) {
    const char* key = "i";
#define CHECK(chr,typ) if constexpr (std::is_same<T, typ>()) { key = chr; }

    CHECK("b", unsigned char);
    CHECK("h", short int);
    CHECK("H", unsigned short int);
    CHECK("i", int);
    CHECK("I", unsigned int);
    CHECK("l", long int);
    CHECK("k", unsigned long);
    CHECK("L", long long);
    CHECK("K", unsigned long long);
    CHECK("n", Py_ssize_t);
    CHECK("c", char);
    CHECK("f", float);
    CHECK("d", double);
    CHECK("D", Py_complex);

#undef CHECK
    return Py_BuildValue(key, val);
}

// this is so evil wtf
template PyObject* PyRuntime::makeInt<unsigned char>(unsigned char);
template PyObject* PyRuntime::makeInt<short int>(short int);
template PyObject* PyRuntime::makeInt<unsigned short int>(unsigned short int);
template PyObject* PyRuntime::makeInt<int>(int);
template PyObject* PyRuntime::makeInt<unsigned int>(unsigned int);
template PyObject* PyRuntime::makeInt<long int>(long int);
template PyObject* PyRuntime::makeInt<unsigned long>(unsigned long);
template PyObject* PyRuntime::makeInt<long long>(long long);
template PyObject* PyRuntime::makeInt<unsigned long long>(unsigned long long);
template PyObject* PyRuntime::makeInt<char>(char);
template PyObject* PyRuntime::makeInt<float>(float);
template PyObject* PyRuntime::makeInt<double>(double);
template PyObject* PyRuntime::makeInt<Py_complex>(Py_complex);

PyObject* PyRuntime::makeBool(bool val) {
    return Py_BuildValue("p", (int)val);
}

PyObject* PyRuntime::makeStr(const std::string_view val) {
    MAKE_C_STRING(val, string);

    auto pystr = Py_BuildValue("s", string);

    FREE_C_STRING(string);

    return pystr;
}

PyObject* PyRuntime::makeStr(const char* val) {
    return Py_BuildValue("s", val);
}