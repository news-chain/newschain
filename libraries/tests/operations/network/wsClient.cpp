//
// Created by boy on 18-6-26.
//


#include "wsClient.hpp"

namespace http{

    client::client(std::string url):_url(url) {

    }


    client::~client() {

    }
	void client::init()
	{
		_client.clear_access_channels(websocketpp::log::alevel::all);
		_client.clear_error_channels(websocketpp::log::alevel::all);


		_client.set_open_handler([](websocketpp::connection_hdl hdl){
		     std::cout << "set_open_handler " << std::endl;
		 });
	

		  _client.set_message_handler([&](websocketpp::connection_hdl hdl, websocket_client_type::message_ptr msg){
//		             std::cout << msg->get_payload() << std::endl;
//						 auto rev = msg->get_payload();
			  if(_cb){
			  	_cb(msg->get_payload());
			  }
		  });

		 _client.set_close_handler([](websocketpp::connection_hdl hdl){
		    std::cout << "set_close_handler" << std::endl;
		 });
	

		_client.set_fail_handler([](websocketpp::connection_hdl hdl){
		    std::cout << "set_fail_handler" << std::endl;
		 });
		

		_client.init_asio();
		_client.start_perpetual();
		_client_thread = std::thread([this]() {
			_client.run();
		});

		websocketpp::lib::error_code ec;
		auto con = _client.get_connection(_url, ec);
		if (ec) {
			std::cout << "error : " << ec << std::endl;
		}

		_connection = _client.connect(con);
	}

    void client::init(open_handler h, message_handler h1, close_handler h2, fail_handler h3)
	{ 
        _client.clear_access_channels(websocketpp::log::alevel::all);
        _client.clear_error_channels(websocketpp::log::alevel::all);		
		_client.set_open_handler(h);
		_client.set_message_handler(h1);
		_client.set_close_handler(h2);
		_client.set_fail_handler(h3);   
 
        _client.init_asio();
        _client.start_perpetual();
        _client_thread = std::thread([this](){
            _client.run();
        });

        websocketpp::lib::error_code ec;
        auto con = _client.get_connection(_url, ec);
        if(ec){
            std::cout << "error : " << ec << std::endl;
        } 
        _connection = _client.connect(con); 
    }

    void client::send_message(std::string msg) {
        if(_connection->get_state() == websocketpp::session::state::open){
            auto ec = _connection->send(msg);
            if(ec){
                std::cout << "send message error " << ec << std::endl;
            }
        }
    }


     
	void client::stop()
	{
		_client.stop();
		_client_thread.join();
	}

    void client::set_handle_message(std::function<void(std::string msg)> cb) {
        _cb = cb;
    }


}