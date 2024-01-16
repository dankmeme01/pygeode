#include "gd.hpp"
#include "shared_p.hpp"

using namespace geode::prelude;

PyMODINIT_FUNC PyInit__gd() {
    auto& rt = PyRuntime::get();

    PyObject* module = PyModule_Create(&module::_gd::def);

    if (module == nullptr) {
        geode::log::error("failed to initialize the python module for gd");
        return nullptr;
    }

    // set attributes

    return module;
}

namespace module::_gd {
    MAKE_CALLER_NSV(CCObject, retain);
    MAKE_CALLER_NSV(CCObject, release);
    MAKE_CALLER_NSNVCQ(CCObject, retainCount, unsigned int);

    PyObject* FLAlertLayer_create(PyObject* self, PyObject* args) {
        auto& rt = PyRuntime::get();
        CHECK_ARGS(args, 3);
        auto title = rt.unpackFromTuple<std::string>(args, 0);
        auto desc = rt.unpackFromTuple<std::string>(args, 1);
        auto button = rt.unpackFromTuple<std::string>(args, 2);
        auto popup = FLAlertLayer::create(title.c_str(), desc, button.c_str());

        return rt.makeInt<uintptr_t>((uintptr_t)popup);
    }

    MAKE_CALLER_NSV(FLAlertLayer, show);
}