//
// Created by boy on 18-7-5.
//

#include <app/logs.hpp>

namespace news {
    namespace app {

        void set_logging_program_options(boost::program_options::options_description &options) {
            std::vector<std::string> default_console_appender({"{\"appender\":\"stderr\",\"stream\":\"std_error\"}"});
            std::string str_default_console_appender = boost::algorithm::join(default_console_appender, " ");

            std::vector<std::string> default_file_appender({"{\"appender\":\"p2p\",\"file\":\"logs/p2p/p2p.log\"}"});
            std::string str_default_file_appender = boost::algorithm::join(default_file_appender, " ");

            std::vector<std::string> default_logger(
                    {"{\"name\":\"default\",\"level\":\"all\",\"appender\":\"stderr\"}\n",
                     "{\"name\":\"p2p\",\"level\":\"all\",\"appender\":\"p2p\"}"}
                     );
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


        fc::optional <fc::logging_config>
        load_logging_config(const boost::program_options::variables_map &args, const boost::filesystem::path &pwd) {
            try {
                fc::logging_config logging_config;
                bool found_logging_config = false;

                if (args.count("log-console-appender")) {
                    std::vector <std::string> console_appenders =
                            args["log-console-appender"].as < std::vector < std::string > > ();

                    for (std::string &s : console_appenders) {
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
                    std::vector <std::string> file_appenders = args["log-file-appender"].as < std::vector < std::string > > ();

                    for (std::string &s : file_appenders) {
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
                    std::vector <std::string> loggers = args["log-logger"].as < std::vector < std::string > > ();

                    for (std::string &s : loggers) {
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
    }

}