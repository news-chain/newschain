//
// Created by boy on 18-6-26.
//

#pragma once


#include <fc/network/http/websocket.hpp>
#include <thread>
#include <websocketpp/config/asio.hpp>
#include <websocketpp/endpoint.hpp>
#include <websocketpp/logger/stub.hpp>
#include <websocketpp/client.hpp> 

namespace http{

    using namespace websocketpp;


    struct asio_with_stub_log : public websocketpp::config::asio {

        typedef asio_with_stub_log type;
        typedef asio base;

        typedef base::concurrency_type concurrency_type;

        typedef base::request_type request_type;
        typedef base::response_type response_type;

        typedef base::message_type message_type;
        typedef base::con_msg_manager_type con_msg_manager_type;
        typedef base::endpoint_msg_manager_type endpoint_msg_manager_type;

        /// Custom Logging policies
        /*typedef websocketpp::log::syslog<concurrency_type,
            websocketpp::log::elevel> elog_type;
        typedef websocketpp::log::syslog<concurrency_type,
            websocketpp::log::alevel> alog_type;
        */
        //typedef base::alog_type alog_type;
        //typedef base::elog_type elog_type;
        typedef websocketpp::log::stub elog_type;
        typedef websocketpp::log::stub alog_type;

        typedef base::rng_type rng_type;

        struct transport_config : public base::transport_config {
            typedef type::concurrency_type concurrency_type;
            typedef type::alog_type alog_type;
            typedef type::elog_type elog_type;
            typedef type::request_type request_type;
            typedef type::response_type response_type;
            typedef websocketpp::transport::asio::basic_socket::endpoint
                    socket_type;
        };

        typedef websocketpp::transport::asio::endpoint<transport_config>
                transport_type;

        static const long timeout_open_handshake = 0;
    };


    typedef websocketpp::client<asio_with_stub_log> websocket_client_type;
//    typedef std::shared_ptr<websocket_connection> websocket_connection_ptr;
	typedef websocket_client_type::message_handler message_handler;
    typedef websocket_client_type::message_ptr message_ptr;
    class client{
    public:
        client(std::string url);
        client(const client&c){
//            *this = c;
            this->_url = c._url;
        }
        client(){}
        ~client();
		void init();
		void init(open_handler h, message_handler h1, close_handler h2, fail_handler h3);
        void send_message(std::string msg); 
		void stop();

        void set_handle_message(std::function<void(std::string msg)> cb);
    private:

        std::string _url;
        std::thread _client_thread;
        websocket_client_type   _client;
//        websocket_connection_ptr _connection;
        websocket_client_type::connection_ptr _connection;

        std::function<void(std::string msg)> _cb;
    };
}