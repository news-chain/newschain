//
// Created by boy on 18-5-31.
//






#include <news/plugins/webserver/webserver_plugin.hpp>

#include <fc/log/logger_config.hpp>
#include <fc/exception/exception.hpp>
#include <fc/network/ip.hpp>
#include <fc/network/resolve.hpp>

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/config/asio.hpp>

#include <boost/optional.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <websocketpp/server.hpp>


namespace news{
    namespace plugins{
        namespace webserver{




            namespace asio = boost::asio;

            using std::map;
            using std::string;
            using boost::optional;
            using boost::asio::ip::tcp;
            using std::shared_ptr;
            using websocketpp::connection_hdl;


            typedef uint32_t thread_pool_size_t;


            namespace detail{

                struct asio_with_stub_log : public websocketpp::config::asio
                {
                    typedef asio_with_stub_log type;
                    typedef asio base;

                    typedef base::concurrency_type concurrency_type;

                    typedef base::request_type request_type;
                    typedef base::response_type response_type;

                    typedef base::message_type message_type;
                    typedef base::con_msg_manager_type con_msg_manager_type;
                    typedef base::endpoint_msg_manager_type endpoint_msg_manager_type;

                    typedef base::alog_type alog_type;
                    typedef base::elog_type elog_type;
                    //typedef websocketpp::log::stub elog_type;
                    //typedef websocketpp::log::stub alog_type;

                    typedef base::rng_type rng_type;

                    struct transport_config : public base::transport_config
                    {
                        typedef type::concurrency_type concurrency_type;
                        typedef type::alog_type alog_type;
                        typedef type::elog_type elog_type;
                        typedef type::request_type request_type;
                        typedef type::response_type response_type;
                        typedef websocketpp::transport::asio::basic_socket::endpoint
                                socket_type;
                    };

                    typedef websocketpp::transport::asio::endpoint< transport_config >
                            transport_type;

                    static const long timeout_open_handshake = 0;
                };

                using websocket_server_type = websocketpp::server< detail::asio_with_stub_log >;

                std::vector<fc::ip::endpoint> resolve_string_to_ip_endpoints( const std::string& endpoint_string )
                {
                    try
                    {
                        string::size_type colon_pos = endpoint_string.find( ':' );
                        if( colon_pos == std::string::npos )
                            FC_THROW( "Missing required port number in endpoint string \"${endpoint_string}\"",
                                      ("endpoint_string", endpoint_string) );

                        std::string port_string = endpoint_string.substr( colon_pos + 1 );

                        try
                        {
                            uint16_t port = boost::lexical_cast< uint16_t >( port_string );
                            std::string hostname = endpoint_string.substr( 0, colon_pos );
                            std::vector< fc::ip::endpoint > endpoints = fc::resolve( hostname, port );

                            if( endpoints.empty() )
                                FC_THROW_EXCEPTION( fc::unknown_host_exception, "The host name can not be resolved: ${hostname}", ("hostname", hostname) );

                            return endpoints;
                        }
                        catch( const boost::bad_lexical_cast& )
                        {
                            FC_THROW("Bad port: ${port}", ("port", port_string) );
                        }
                    }
                    FC_CAPTURE_AND_RETHROW( (endpoint_string) )
                }



                class webserver_plugin_impl{
                public:
                    webserver_plugin_impl(thread_pool_size_t size)
                    :thread_pool_work(this->thread_pool_io)
                    {
                        for(thread_pool_size_t i = 0; i < size; i++){
                            thread_pool.create_thread(boost::bind( &asio::io_service::run, &thread_pool_io));
                        }
                    }

                    void start_webserver();
                    void stop_webserver();

                    void handle_ws_message(websocket_server_type *server, connection_hdl hdl, detail::websocket_server_type::message_ptr);
                    void handle_http_message(websocket_server_type *server, connection_hdl hdl);

                    shared_ptr<std::thread>     http_thread;
                    asio::io_service            http_ios;
                    optional<tcp::endpoint>     http_endpoint;
                    websocket_server_type       http_server;


                    shared_ptr<std::thread>     ws_thread;
                    asio::io_service            ws_ios;
                    optional<tcp::endpoint>     ws_endpoint;
                    websocket_server_type       ws_server;

                    boost::thread_group         thread_pool;
                    asio::io_service            thread_pool_io;
                    asio::io_service::work      thread_pool_work;
                };

                void webserver_plugin_impl::handle_ws_message(
                        news::plugins::webserver::detail::websocket_server_type *server,
                        websocketpp::connection_hdl hdl,
                        std::shared_ptr<websocketpp::message_buffer::message<websocketpp::message_buffer::alloc::con_msg_manager>>) {

                }

                void webserver_plugin_impl::handle_http_message(
                        news::plugins::webserver::detail::websocket_server_type *server,
                        websocketpp::connection_hdl hdl) {

                    auto con = server->get_con_from_hdl(hdl);
                    con->defer_http_response();
                    thread_pool_io.post( [con, this](){
                        try {
                            auto body = con->get_request_body();
                            con->set_body(body);
                            con->set_status( websocketpp::http::status_code::ok);
                        }catch (fc::exception &e){
                            edump((e));
                        }catch (...){
                            auto eptr = std::current_exception();
                            try {
                                if(eptr){
                                    std::rethrow_exception(eptr);
                                }
                                con->set_body("unknown error occurred");
                                con->set_status( websocketpp::http::status_code::internal_server_error);
                            }catch (const std::exception &e){
                                std::stringstream ss;
                                ss << "unknown exception: " << e.what();
                                con->set_body( ss.str() );
                                con->set_status( websocketpp::http::status_code::internal_server_error);
                            }

                        }
                        con->send_http_response();

                    });



                }



                void webserver_plugin_impl::start_webserver() {
                    if(ws_endpoint){
                        ws_thread = std::make_shared<std::thread>([&](){
                            ilog("start ws thread");
                            try {
                                ws_server.clear_access_channels(websocketpp::log::alevel::all);
                                ws_server.clear_error_channels(websocketpp::log::alevel::all);

                                ws_server.set_message_handler(boost::bind(&webserver_plugin_impl::handle_ws_message, this, &ws_server, _1, _2));
                                //TODO: http_endpoint == ws_endpoint?

                                ws_server.listen(*ws_endpoint);
                                ws_server.start_accept();

                                ws_ios.run();
                                ilog("complete ws_ios listening");
                            }catch (...){
                                elog("catch erro in ws_server");
                            }


                        });
                    }

                    if(http_endpoint){
                        http_thread = std::make_shared<std::thread>([&](){
                            try {
//                            ilog("start http thread");
                                http_server.clear_error_channels(websocketpp::log::alevel::all);
                                http_server.clear_access_channels(websocketpp::log::alevel::all);
                                http_server.init_asio(&http_ios);
                                http_server.set_reuse_addr(true);

                                http_server.set_http_handler( boost::bind(&webserver_plugin_impl::handle_http_message, this, &http_server, _1) );

                                ilog("start listening for http requests");

                                http_server.listen(*http_endpoint);
                                http_server.start_accept();

                                http_ios.run();

//                                wlog("complete http_server listening");

                            }catch (fc::exception &e){
                                elog("error throw exception for http_endpoint start:   ${e}", ("e", e.what()));
                            }catch (std::exception &e){
                                elog("error throw exception for http_endpoint start:   ${e}", ("e", e.what()));
                            }

                        });
                    }
                }

                void webserver_plugin_impl::stop_webserver() {
                    if(ws_server.is_listening()){
                        ws_server.stop_listening();
                    }

                    if(http_server.is_listening()){
                        http_server.stop_listening();
                    }

                    thread_pool_io.stop();
                    thread_pool.join_all();

                    if(ws_thread){
                        ws_ios.stop();
                        ws_thread->join();
                        ws_thread.reset();
                    }

                    if(http_thread){
                        http_ios.stop();
                        http_thread->join();
                        http_thread.reset();
                    }
                    elog("webserver stop OK!");
                }



            } //namespace detail




            webserver_plugin::webserver_plugin(){
            }


            webserver_plugin::~webserver_plugin(){

            }



            void webserver_plugin::set_program_options(boost::program_options::options_description &cli,
                                                       boost::program_options::options_description &cfg) {
                cfg.add_options()
                        ("webserver-http-endpoint", bpo::value<std::string>(), "http endpoint for webserver requests")
                        ("webserver-ws-endpoint", bpo::value<std::string>(), "websocket endpoint for webserver requests")
                        ("webserver-thread-pool-size", bpo::value<thread_pool_size_t >()->default_value(32), "number of threads userd to headle quires, default:32");
            }

            void webserver_plugin::plugin_initialize(const boost::program_options::variables_map &options) {
                try{
                    auto thread_pool_size = options.at("webserver-thread-pool-size").as<thread_pool_size_t>();
                    FC_ASSERT(thread_pool_size > 0, "thread_pool_size must be ");
                    ilog("init websocket thread pool size ${s}", ("s", thread_pool_size));

                    my.reset(new detail::webserver_plugin_impl(thread_pool_size));

                    if(options.count("webserver-http-endpoint")){
                        auto http_endpoint = options.at("webserver-http-endpoint").as<std::string>();
                        auto endpoints = detail::resolve_string_to_ip_endpoints( http_endpoint );
                        FC_ASSERT(endpoints.size() > 0, "webserver-http-endpoint ${h} did not resolve", ("h", endpoints));
                        my->http_endpoint = tcp::endpoint( boost::asio::ip::address_v4::from_string( (string)endpoints[0].get_address()), endpoints[0].port());
                        ilog("configured http to listen on ${e}", ("e", endpoints[0]));
                    }

                    if( options.count( "webserver-ws-endpoint" ) )
                    {
                        auto ws_endpoint = options.at( "webserver-ws-endpoint" ).as< string >();
                        auto endpoints = detail::resolve_string_to_ip_endpoints( ws_endpoint );
                        FC_ASSERT( endpoints.size(), "ws-server-endpoint ${hostname} did not resolve", ("hostname", ws_endpoint) );
                        my->ws_endpoint = tcp::endpoint( boost::asio::ip::address_v4::from_string( ( string )endpoints[0].get_address() ), endpoints[0].port() );
                        ilog( "configured ws to listen on ${ep}", ("ep", endpoints[0]) );
                    }
                }catch (...){
                    elog("webserver_plugin init error ");
                }




            }

            void webserver_plugin::plugin_startup() {
                my->start_webserver();

            }

            void webserver_plugin::plugin_shutdown() {
                my->stop_webserver();
            }




        }//webserver
    }//plugins
}//news