#include "Navi.hpp"
#include "argparse.hpp"

constexpr std::string VERSION = "1.0";

int main(int argc, char *argv[])
{
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

    // Path
    parser.add_argument("--working-directory", "-D")
.help("Path to one (or two) directory to open single (dual) pane")
        .nargs(2);


    try {
        parser.parse_args(argc, argv);
    } catch (std::exception &err) {
        std::cerr << err.what() << "\n";
        std::cerr << parser;
        std::exit(1);
    }

    Navi navi;
    navi.readArgumentParser(parser);
    navi.initThings();
    app.exec();
}