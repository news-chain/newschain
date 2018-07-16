//
// Created by boy on 18-6-20.
//

#include <fc/exception/exception.hpp>
#include <iostream>
#include <wsClient.hpp>
#include <fc/io/json.hpp>
#include "factory.hpp"

#include<thread>
#include <vector>
#include <map> 
#include <random> 
 
struct result_body {
	std::string jsonrpc;
	fc::optional<std::string> result;
	fc::optional<std::string> error;
	int64_t     id;
};

enum option_user
{
	create_user = 0x1,
	transfer_one = create_user + 1,
	transfer_ones = transfer_one + 1
};
enum task_result
{
	undeal = 0x1,
	sendfail = undeal + 0x1,
	recvfail = sendfail + 0x1,
	fail = undeal + 1,
	suces= fail+1
};
struct taskresult
{
	uint64_t id;
	task_result result;
	uint64_t start_time;
	uint64_t end_time;
};

typedef std::map<int64_t, taskresult> tasklist;
 
FC_REFLECT(result_body, (jsonrpc)(result)(error)(id))
 
using namespace factory;
 

typedef std::vector<std::thread*> thread_pool;
typedef std::map<news::base::account_name, news::base::private_key_type> users;
class business
{
public:
	business(std::string &ws_,int thread_count_=8) :wsaddress(ws_), thread_counts(thread_count_),isstop(false)
	{ 
		myusers.insert(std::make_pair(1, NEWS_INIT_PRIVATE_KEY));
		myusers_count = 1;
		request_ack = 0;
	}
	void open_handler(websocketpp::connection_hdl h)
	{
		std::cout << "set_open_handler " <<std::endl; 
	}
	bool gen_account(int count=20)
	{  
		if (count == 0)
			bool always = true; 
			assert(thread_counts>0);
			auto start = fc::time_point::now();
			for (int i = 0; i < thread_counts; i++) {
				std::thread* th = new std::thread([&]() {
					http::client client(wsaddress); 
					client.init(
						[](websocketpp::connection_hdl hdl)
					{
						std::cout << "set_open_handler " << std::endl;
					}, 
						[&](websocketpp::connection_hdl hdl, http::message_ptr msg)
					{
						result_body body = fc::json::from_string(msg->get_payload()).as<result_body>();
						if (body.error.valid())
						{ 

						} 
					}, 
						[](websocketpp::connection_hdl hdl) 
					{
						std::cout << "set_close_handler " << std::endl;
					}, 
						[](websocketpp::connection_hdl hdl) 
					{
						std::cout << "set_fail_handler " << std::endl;
					});
					auto ff = factory::helper();
					srand((unsigned)time(NULL));
					while (!isstop&&count==0)
					{
						std::random_device rd;
						std::default_random_engine engine(rd());
						{
							std::lock_guard<std::mutex> lock(mutex_);
							myusers_count = myusers.size();
							if (count != 0 && myusers_count > count - 1)
								break;
						}
						std::uniform_int_distribution<> dis(0, myusers_count - 1);
						auto  productor = std::bind(dis, engine);
						int from = productor();  
						int genuser;
						news::base::private_key_type genkey;
						auto str = ff.create_account(myusers.at(from), (account_name&)from, (account_name&)genuser, genkey);
						std::string ret = string_json_rpc(fc::json::to_string(str));
						client.send_message(ret);
					}
					auto end = fc::time_point::now();
					ilog("time:${t}", ("t", end - start));
					client.start();
				});
				th_pool.push_back(th);
			} 
		myusers_count = myusers.size();
		return true;
	} 

	bool start_business()
	{
		myusers.clear();
		gen_account(30); 
		end_business();
		assert(thread_counts>0);
		auto start = fc::time_point::now();
		for (int i = 0; i < thread_counts; i++) {
			std::thread* th=new std::thread([&]() {
				http::client client(wsaddress);
				client.init();   
				auto ff = factory::helper();
				srand((unsigned)time(NULL));
				while(!isstop) 
				{
					std::random_device rd;
					std::default_random_engine engine(rd());
					std::uniform_int_distribution<> dis(0, myusers_count-1);
					auto  productor = std::bind(dis, engine);
					int from = productor(); 
					int to = productor();   
					auto money=asset(NEWS_SYMBOL, productor()*100000000L); 
					auto str = ff.create_transfer(myusers.at(from), (account_name&)from, (account_name&)to, money);
					std::string ret = string_json_rpc(fc::json::to_string(str));
					client.send_message(ret);
				}
				auto end = fc::time_point::now();
				ilog("time:${t}", ("t", end - start));
				client.start();
			});
			th_pool.push_back(th);
		}
		return true;

	}
	bool end_business()
	{ 
		isstop = true;
		for (auto it : th_pool)
		{
			it->join();
			delete it;
		}
	}
private:
	thread_pool th_pool; 
	int thread_counts;
	bool isstop;
	std::string wsaddress;
	
	users myusers; 
	int myusers_count; 
	std::mutex mutex_;


	uint64_t request_ack; 

	tasklist mytask;



	
};



int main(int argc, char **argv){

	std::string addr = "ws://192.168.2.174:7771";
	business bs(addr,1);
	bs.gen_account(); 
    return 0;
}


