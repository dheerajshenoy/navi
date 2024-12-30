#include <QStandardPaths>

#include "argparse.hpp"
#include "utils.hpp"

constexpr std::string VERSION = "1.2";
// Config related things

#include "API.hpp"

#include "Globals.hpp"
#include "Navi.hpp"
#include <QFile>

int main(int argc, char *argv[]) {
    argparse::ArgumentParser parser("navi", VERSION);
    QApplication app(argc, argv);

    //////////////////////
    // Argument options //
    //////////////////////

    // Config
    parser.add_argument("--config", "-c")
        .help("Path to a lua config file to use")
        .nargs(1);

    // No config
    parser.add_argument("--quick", "-Q")
        .help("Load navi without loading configuration file")
        .default_value(false)
        .implicit_value(true)
        .flag();

    // Bookmark file
    parser.add_argument("--bookmark-file", "-B")
        .help("Load a bookmark file")
        .nargs(1);

    parser.add_argument("files")
        .help("Directory to load")
        .remaining();

    try {
        parser.parse_args(argc, argv);
    } catch (std::exception &err) {
        std::cerr << err.what() << "\n";
        std::exit(1);
    }

    sol::state lua;

    init_lua_api(lua);

    Navi *navi = nullptr;

    lua.set_function("_store_instance", [&navi](Navi *instance) {
        navi = instance;
    });

    if (QFile::exists(PRIVATE_API_FILE_PATH.at(0))) {
        try {
            lua.script_file(PRIVATE_API_FILE_PATH.at(0).toStdString(), sol::load_mode::any);
            lua.script("navi = require('navi')");
        } catch (sol::error &e) {
            std::cerr << "Could not load Navi API files: " << e.what() << "\n";
        }
    } else if (QFile::exists(PRIVATE_API_FILE_PATH.at(1))) {
        try {
            lua.script_file(PRIVATE_API_FILE_PATH.at(1).toStdString(), sol::load_mode::any);
            lua.script("navi = require('navi')");
        } catch (sol::error &e) {
            std::cerr << "Could not load Navi API files: " << e.what() << "\n";
        }
    } else {
        std::cerr << "Navi Lua API files are missing, they are required for the software to work correctly. Please re-install the program.\n";
        std::exit(-1);
    }

    if (!navi) {
        std::cerr << "Could not create navi on the heap" << "\n";
        return -1;
    }

    navi->update_runtime_paths(lua["package"]["path"]);
    navi->readArgumentParser(parser);
    navi->initThings();

    try {
        lua.safe_script_file(CONFIG_FILE_PATH.toStdString(), sol::load_mode::any);
    } catch (sol::error &e) {
        std::cerr << "Could not load user config file: " << e.what() << "\n";
    }

    app.exec();
}
