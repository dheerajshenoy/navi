#include <string>
constexpr std::string VERSION = "1.2.4";

#include <QFile>
#include <QStandardPaths>
#include "argparse.hpp"
#include "utils.hpp"
#include "API.hpp"
#include "Globals.hpp"
#include "Navi.hpp"

void read_lua_file(sol::state &lua) {

    auto temp = [&](const QString &_path) noexcept {
        std::string path = _path.toStdString();
        std::vector<char> bytecode = utils::readBinaryFile(path);
        sol::load_result script = lua.load_buffer(bytecode.data(),
                                                  bytecode.size(),
                                                  path.c_str());

        sol::protected_function_result result = script();

        /*lua.script_file(PRIVATE_API_FILE_PATH.at(0).toStdString(), sol::load_mode::any);*/
        lua.script("navi = require('navi')");
        lua.script(R"(
function getTableMembers(tbl, prefix)
   prefix = prefix or "" -- Initialize prefix if not provided
   results = {} -- Initialize results if not provided

   for key, value in pairs(tbl) do
       local fullName = prefix .. key

       if type(value) == "table" then
       -- If the value is a table, recurse into it with updated prefix
       getTableMembers(value, fullName .. ".", results)
       else
       -- Add the fully resolved name to the results
       results[#results + 1] = fullName
       end
   end

   return results
end

_navi_api_list = getTableMembers(navi, "navi.")
               )");
    };

    if (QFile::exists(PRIVATE_API_FILE_PATH.at(0))) {
        try {
            temp(PRIVATE_API_FILE_PATH.at(0));
        } catch (sol::error &e) {
            std::cerr << "Could not load Navi API files: " << e.what() << "\n";
        }
    } else if (QFile::exists(PRIVATE_API_FILE_PATH.at(1))) {
        try {
            temp(PRIVATE_API_FILE_PATH.at(1));
        } catch (sol::error &e) {
            std::cerr << "Could not load Navi API files: " << e.what() << "\n";
        }
    } else {
        std::cerr << "Navi Lua API files are missing, they are required for the software to work correctly. Please re-install the program.\n";
        std::exit(-1);
    }
}

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

    read_lua_file(lua);

    if (!navi) {
        std::cerr << "Could not create navi on the heap" << "\n";
        return -1;
    }

    std::vector<std::string> result;
    utils::getTableMembers(lua["navi"], "navi.", result);
    QStringList api_list = utils::stringListFromVector(result);
    api_list.removeAll("");
    navi->set_api_list(api_list);
    navi->update_runtime_paths(lua["package"]["path"]);
    navi->readArgumentParser(parser);
    navi->initThings();
    navi->set_lua_state(lua);

    try {
        lua.safe_script_file(CONFIG_FILE_PATH.toStdString(), sol::load_mode::any);
    } catch (sol::error &e) {
        std::cerr << "Could not load user config file: " << e.what() << "\n";
    }

    app.exec();
}

