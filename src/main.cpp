#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

#include <pyruntime/pyruntime.hpp>
#include <pyruntime/modules/geode.hpp>
#include <pyruntime/modules/gd.hpp>

using namespace geode::prelude;

constexpr std::string_view PYGEODE_LIB_REV = "1";

$execute {
    // check for python libraries
    auto resourceDir = Mod::get()->getResourcesDir();
    auto libDir = resourceDir.parent_path().parent_path().parent_path().parent_path().parent_path() / "Lib";

    if (!ghc::filesystem::exists(libDir)) {
        (void) utils::file::createDirectoryAll(libDir).expect("could not create the directory Lib: {}").unwrap();
    }

    auto revFile = libDir / "_pygeode_rev";
    bool skipExtracting = false;

    if (ghc::filesystem::exists(revFile)) {
        skipExtracting = utils::file::readString(revFile) == PYGEODE_LIB_REV;
    }

    if (!skipExtracting) {
        ghc::filesystem::remove_all(libDir);
        (void) utils::file::createDirectoryAll(libDir).expect("could not create the directory Lib: {}").unwrap();

        // extract the zip file into the folder
        auto zipFile = resourceDir / "Lib.zip";
        (void) utils::file::Unzip::intoDir(zipFile, libDir).expect("could not unzip the file Lib.zip: {}").unwrap();
    }

    if (PyImport_AppendInittab("_geode", PyInit__geode) == -1) {
        throw std::runtime_error("failed to append inittab for module '_geode'");
    }

    if (PyImport_AppendInittab("_gd", PyInit__gd) == -1) {
        throw std::runtime_error("failed to append inittab for module '_gd'");
    }

    // initialize the runtime.
    auto& rt = PyRuntime::get();
    rt.appendPath(resourceDir.string());
}

class $modify(MenuLayer) {
    void onMoreGames(CCObject*) {
        FLAlertLayer::create("Geode", "Hello from my custom mod!", "OK")->show();
        auto resourceDir = Mod::get()->getResourcesDir();

        auto& rt = PyRuntime::get();
        rt.runStringFromFile(resourceDir / "example.py");
    }
};
