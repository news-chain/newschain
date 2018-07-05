//
// Created by boy on 18-7-4.
//

#pragma once


#include <fc/exception/exception.hpp>
#include <fc/log/logger_config.hpp>
#include <fc/log/logger.hpp>
#include <fc/exception/exception.hpp>
#include <fc/reflect/reflect.hpp>
#include <fc/reflect/variant.hpp>
#include <fc/io/json.hpp>
#include <fc/log/console_appender.hpp>
#include <fc/log/file_appender.hpp>

#include <vector>
#include <string>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/filesystem/path.hpp>

#include <boost/algorithm/string.hpp>

namespace news {
    namespace app {


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


        void set_logging_program_options(boost::program_options::options_description &options) ;
        fc::optional <fc::logging_config> load_logging_config(const boost::program_options::variables_map &args, const boost::filesystem::path &pwd);
    }

}



FC_REFLECT(news::app::console_appender_args, (appender)(stream))
FC_REFLECT(news::app::file_appender_args, (appender)(file))
FC_REFLECT(news::app::logger_args, (name)(level)(appender))