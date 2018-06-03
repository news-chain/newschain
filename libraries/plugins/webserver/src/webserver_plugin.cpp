//
// Created by boy on 18-5-31.
//




#include <news/plugins/webserver/webserver_plugin.hpp>


namespace news{
    namespace plugins{
        namespace webserver{
            namespace detail{
                class webserver_plugin_impl{
                public:
                    webserver_plugin_impl(){

                    }

                };
            }




            webserver_plugin::webserver_plugin():my(new detail::webserver_plugin_impl()){

            }


            webserver_plugin::~webserver_plugin(){

            }


            void webserver_plugin::set_program_options(boost::program_options::options_description &cli,
                                                       boost::program_options::options_description &cfg) {
                cfg.add_options()
                        ("webserver-http-endpoint", bpo::value<std::string>(), "http endpoint for webserver requests")
                        ("webserver-ws-endpoint", bpo::value<std::string>(), "websocket endpoint for webserver requests")
                        ("webserver-thread-poll-size", bpo::value<uint32_t >()->default_value(32), "number of threads userd to headle quires, default:32");
            }

            void webserver_plugin::plugin_initialize(const boost::program_options::variables_map &options) {

            }

            void webserver_plugin::plugin_startup() {

            }

            void webserver_plugin::plugin_shutdown() {

            }




        }//webserver
    }//plugins
}//news