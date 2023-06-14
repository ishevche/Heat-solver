// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <iostream>
#include <exceptions.h>
#include <options_parser.h>

namespace po = boost::program_options;

command_line_options_t::command_line_options_t() {
    opt_conf.add_options()
            ("help,h",
             "Show help message")
            ("conf,c", po::value<std::string>(&config_file)->required(),
             "Path to configuration file");
    popt_conf.add("conf", 1);
}

command_line_options_t::command_line_options_t(int ac, char **av) :
        command_line_options_t() {
    parse(ac, av);
}

void command_line_options_t::parse(int ac, char **av) {
    try {
        auto parsed = po::command_line_parser(ac, av)
                .options(opt_conf).positional(popt_conf).run();
        po::store(parsed, var_map);
        if (var_map.count("help")) {
            std::cout << opt_conf << "\n";
            exit(EXIT_SUCCESS);
        }
        po::notify(var_map);
    } catch (std::exception &ex) {
        throw ArgumentsException(ex.what());
    }
}

config_file_options_t::config_file_options_t() {
    opt_conf.add_options()
            ("width", po::value<double>(&config.width)->required())
            ("height", po::value<double>(&config.height)->required())
            ("x_steps", po::value<int>(&config.x_steps)->required())
            ("y_steps", po::value<int>(&config.y_steps)->required())
            ("t_steps", po::value<size_t>(&config.t_steps)->required())
            ("display_interval",
             po::value<size_t>(&config.display_interval)->required())
            ("init_file", po::value<std::string>(&config.init_file)->required())
            ("pngs_folder",
                    po::value<std::string>(&config.pngs_folder)->required())
            ("min_temp", po::value<double>(&config.min_temp)->required())
            ("max_temp", po::value<double>(&config.max_temp)->required())
            ("delta_t", po::value<double>(&config.delta_t)->required())
            ("density", po::value<double>(&config.density)->required())
            ("conduction", po::value<double>(&config.conduction)->required())
            ("heat_capacity",
             po::value<double>(&config.heat_capacity)->required());
}

config_file_options_t::config_file_options_t(const std::string &config_file) :
        config_file_options_t() {
    parse(config_file);
}

void config_file_options_t::parse(const std::string &config_file) {
    try {
        auto parsed = po::parse_config_file(config_file.c_str(),
                                            opt_conf);
        po::store(parsed, var_map);
        po::notify(var_map);

        remove_brackets(config.init_file);
        remove_brackets(config.pngs_folder);
    } catch (po::reading_file &ex) {
        throw ConfigFileOpenError(ex.what());
    } catch (std::exception &ex) {
        throw FileParseError(ex.what());
    }
}

void config_file_options_t::remove_brackets(std::string &path) {
    size_t size = path.size();
    path = path.substr(1, size - 2);
}
