//
// Created by boy on 18-6-16.
//


#ifdef DEFAULT_LOGGER
# undef DEFAULT_LOGGER
#endif
#define DEFAULT_LOGGER "p2p"


#include <fc/reflect/variant.hpp>
#include <fc/log/logger.hpp>
#include <fc/log/logger_config.hpp>
#include <fc/log/file_appender.hpp>
#include <fc/exception/exception.hpp>
#include <fc/variant.hpp>
#include <iostream>


#include <boost/filesystem.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <fc/io/json.hpp>
#include <fc/log/console_appender.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/asio.hpp>


#include <vector>
#include <boost/program_options.hpp>

using namespace fc;
using namespace std;

struct console_appender_args {
    std::string appender;
    std::string stream;
};

struct file_appender_args {
    std::string appender;
    std::string file;
};

struct logger_args {
    std::string name;
    std::string level;
    std::string appender;
};

FC_REFLECT(console_appender_args, (appender) (stream))
FC_REFLECT(file_appender_args, (appender) (file))
FC_REFLECT(logger_args, (name)(level)(appender))


void set_logging_program_options(boost::program_options::options_description &options) {
    std::vector<std::string> default_console_appender({"{\"appender\":\"stderr\",\"stream\":\"std_error\"}"});
    std::string str_default_console_appender = boost::algorithm::join(default_console_appender, " ");

    std::vector<std::string> default_file_appender({"{\"appender\":\"p2p\",\"file\":\"logs/p2p/p2p.log\"}"});
    std::string str_default_file_appender = boost::algorithm::join(default_file_appender, " ");

    std::vector<std::string> default_logger(
            {"{\"name\":\"default\",\"level\":\"all\",\"appender\":\"stderr\"}\n",
             "{\"name\":\"p2p\",\"level\":\"all\",\"appender\":\"p2p\"}"});
    std::string str_default_logger = boost::algorithm::join(default_logger, "");

    options.add_options()
            ("log-console-appender",
             boost::program_options::value<std::vector<std::string> >()->composing()->default_value(
                     default_console_appender, str_default_console_appender),
             "Console appender definition json: {\"appender\", \"stream\"}")
            ("log-file-appender",
             boost::program_options::value<std::vector<std::string> >()->composing()->default_value(
                     default_file_appender, str_default_file_appender),
             "File appender definition json:  {\"appender\", \"file\"}")
            ("log-logger",
             boost::program_options::value<std::vector<std::string> >()->composing()->default_value(default_logger,
                                                                                                    str_default_logger),
             "Logger definition json: {\"name\", \"level\", \"appender\"}");
}

fc::optional<fc::logging_config>
load_logging_config(const boost::program_options::variables_map &args, const boost::filesystem::path &pwd) {
    try {
        fc::logging_config logging_config;
        bool found_logging_config = false;

        if (args.count("log-console-appender")) {
            std::vector<string> console_appenders = args["log-console-appender"].as<vector<string> >();

            for (string &s : console_appenders) {
                try {
                    auto console_appender = fc::json::from_string(s).as<console_appender_args>();

                    fc::console_appender::config console_appender_config;
                    console_appender_config.level_colors.emplace_back(
                            fc::console_appender::level_color(fc::log_level::debug,
                                                              fc::console_appender::color::green));
                    console_appender_config.level_colors.emplace_back(
                            fc::console_appender::level_color(fc::log_level::warn,
                                                              fc::console_appender::color::brown));
                    console_appender_config.level_colors.emplace_back(
                            fc::console_appender::level_color(fc::log_level::error,
                                                              fc::console_appender::color::red));
                    console_appender_config.stream = fc::variant(
                            console_appender.stream).as<fc::console_appender::stream::type>();
                    logging_config.appenders.push_back(
                            fc::appender_config(console_appender.appender, "console",
                                                fc::variant(console_appender_config)));
                    found_logging_config = true;
                }
                catch (...) {}
            }
        }

        if (args.count("log-file-appender")) {
            std::vector<string> file_appenders = args["log-file-appender"].as<vector<string> >();

            for (string &s : file_appenders) {
                auto file_appender = fc::json::from_string(s).as<file_appender_args>();

                fc::path file_name = file_appender.file;
                if (file_name.is_relative())
                    file_name = fc::absolute(pwd) / file_name;

                // construct a default file appender config here
                // filename will be taken from ini file, everything else hard-coded here
                fc::file_appender::config file_appender_config;
                file_appender_config.filename = file_name;
                file_appender_config.flush = true;
                file_appender_config.rotate = false;
                file_appender_config.rotation_interval = fc::seconds(30);
                file_appender_config.rotation_limit = fc::minutes(1);
                logging_config.appenders.push_back(
                        fc::appender_config(file_appender.appender, "file", fc::variant(file_appender_config)));
                found_logging_config = true;
            }
        }

        if (args.count("log-logger")) {
            std::vector<string> loggers = args["log-logger"].as<std::vector<std::string> >();

            for (string &s : loggers) {
                auto logger = fc::json::from_string(s).as<logger_args>();

                fc::logger_config logger_config(logger.name);
                logger_config.level = fc::variant(logger.level).as<fc::log_level>();
                boost::split(logger_config.appenders, logger.appender,
                             boost::is_any_of(" ,"),
                             boost::token_compress_on);
                logging_config.loggers.push_back(logger_config);
                found_logging_config = true;
            }
        }

        if (found_logging_config)
            return logging_config;
        else
            return fc::optional<fc::logging_config>();
    }
    FC_RETHROW_EXCEPTIONS(warn, "")
}

namespace bpo = boost::program_options;


int main(int argc, char **argv) {

    try {
        bpo::options_description options;

        set_logging_program_options(options);

        boost::program_options::variables_map args;

        bpo::store(bpo::parse_command_line(argc, argv, options), args);

        boost::filesystem::path path = boost::filesystem::current_path();

        fc::optional<fc::logging_config> cfg = load_logging_config(args, path);
        fc::configure_logging(*cfg);


        while (1) {

            ilog("${a}", ("a", "bbbbbbbb"));
            elog("${a}", ("a", "bbbbbbbb"));

            sleep(1);
        }
//        fc::log_context cxt(fc::log_level::all, "123", 123, "123");
//        auto loggg = fc::logger::get("p2p");
//        loggg.log(fc::log_message(cxt, "${s}", fc::variant_object("s", "kkkkk")));

    } catch (const fc::exception e) {
        std::cout << e.to_detail_string() << std::endl;
    }


    return 0;
}