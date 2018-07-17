//
// Created by boy on 18-6-20.
//

#include <fc/exception/exception.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp> 
#include <iostream>
#include <wsClient.hpp>
#include <fc/io/json.hpp>
#include "factory.hpp"

#include<thread>
#include <vector>
#include <map> 
#include <random> 
using namespace boost;
using namespace std;
 
struct result_body {
	std::string jsonrpc;
	fc::optional<fc::variant> result;
	fc::optional<fc::variant> error;
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
	option_user op;
	task_result result;
	uint64_t start_time;
	uint64_t end_time;
	std::string error;
};
enum run_type
{
	balance = 0x1,
	primary = balance + 0x1
};

typedef std::map<int64_t, taskresult> tasklist;
 
FC_REFLECT(result_body, (jsonrpc)(result)(error)(id))
 
using namespace factory;
 

typedef std::vector<std::thread*> thread_pool;
typedef std::map<news::base::account_name, news::base::private_key_type> users;
typedef std::map<news::base::account_name, news::base::private_key_type>::iterator users_it;
class business
{
public:
	business(std::vector<std::string> &ws_, run_type ty= run_type::balance, int thread_count_=8) :wsaddress(ws_), mode(ty),thread_counts(thread_count_),isstop(false)
	{ 
		myusers.insert(std::make_pair(1, NEWS_INIT_PRIVATE_KEY));
		wscount = wsaddress.size();
		wsindex= 0;
		myusers_count = taskid=1;
	}
 
	std::string getws()
	{ 
		if (mode == run_type::primary|| wscount==1)
			return wsaddress[0];
		if (wsindex = wscount)
		{
			wsindex = 0;
			return wsaddress[0];
		}
		return wsaddress[wsindex];

	}

	void update(uint64_t id, task_result fs= task_result::suces)
	{
		mytask[id].result = fs;
	}
	bool befor_transfer_one(int accounts=30)
	{
		http::client client(getws());
		client.init(
			[](websocketpp::connection_hdl hdl)
		{
			std::cout << "on open " << std::endl;
		},
			[&](websocketpp::connection_hdl hdl, http::message_ptr msg)
		{ 
			result_body body = fc::json::from_string(msg->get_payload()).as<result_body>();   
			if(body.error.valid())
				std::cout << "on recv " << msg->get_payload() << std::endl;

		},
			[](websocketpp::connection_hdl hdl)
		{
			std::cout << "on close " << std::endl;
		},
			[](websocketpp::connection_hdl hdl)
		{
			std::cout << "on fail " << std::endl;

		});
		auto ff = factory::helper(); 
	 
		int startid = 10000;
		for (int i=0; i <accounts;i++,startid++)
		{  
			news::base::private_key_type genkey;  
			auto str = ff.create_account(myusers[1], 1, startid, genkey);
			uint64_t id = taskid++;
			std::string ret = string_json_rpc(id, fc::json::to_string(str));
			client.send_message(ret);
			myusers[startid] = genkey;

			auto money = asset(NEWS_SYMBOL, 300 * 10000L);
			ff.create_transfer(myusers[1],1, startid, money); 
			ret = string_json_rpc(id, fc::json::to_string(str));
			client.send_message(ret);

		} 
		client.stop();
		return true;

	}
	bool gen_account()
	{ 
			assert(thread_counts>0); 
			starttime = fc::time_point::now().sec_since_epoch();
			mytask.clear();
			for (int i = 0; i < thread_counts; i++) 
			{
				std::thread* th = new std::thread([&]() 
				{
					http::client client(getws()); 
					client.init(
						[](websocketpp::connection_hdl hdl)
					{
						std::cout << "on open " << std::endl;
					}, 
						[&](websocketpp::connection_hdl hdl, http::message_ptr msg)
					{
						std::cout << msg->get_payload() << std::endl;
						result_body body = fc::json::from_string(msg->get_payload()).as<result_body>(); 
						mytask[body.id].end_time= fc::time_point::now().sec_since_epoch();
						if (body.error.valid())					
							update(body.id, task_result::fail);						
						else
							update(body.id);
						 
					}, 
						[](websocketpp::connection_hdl hdl) 
					{
						std::cout << "on close " << std::endl;
					}, 
						[](websocketpp::connection_hdl hdl) 
					{
						std::cout << "on fail " << std::endl;

					});
					auto ff = factory::helper();
					auto start = fc::time_point::now().sec_since_epoch();				
					while (true)
					{
						if (isstop)
						{
							client.stop();
							break;
						}
						std::random_device rd;
						std::default_random_engine engine(rd());
						{
							std::lock_guard<std::mutex> lock(mutex_);
							myusers_count = myusers.size();						
						}
						std::uniform_int_distribution<> dis(0, myusers_count - 1);
						auto  productor = std::bind(dis, engine);
						uint64_t from = productor()+1;  
						uint64_t genuser=myusers_count+1000;
						news::base::private_key_type genkey;
						auto admin = myusers.lower_bound(from); 
						auto first = admin->first;
						auto str = ff.create_account(admin->second, first,genuser, genkey);
						
					
						uint64_t id = taskid++;
						std::string ret = string_json_rpc(id,fc::json::to_string(str));		
						client.send_message(ret);
						taskresult rs;
						rs.start_time= fc::time_point::now().sec_since_epoch();
						rs.id = id;
						rs.result = task_result::recvfail;
						rs.op = option_user::create_user;
						mytask.insert(std::make_pair(id, std::move(rs)));
						{
							std::lock_guard<std::mutex> lock(mutex_);
							myusers.insert(std::make_pair(genuser, genkey));
						}


					}   
				});
				th_pool.push_back(th);
			} 
		return true;
	} 

	bool start_business(bool multiple=false)
	{ 
		befor_transfer_one();
		myusers_count = myusers.size();
		assert(thread_counts>0);
		starttime = fc::time_point::now().sec_since_epoch();
		mytask.clear();
		for (int i = 0; i < thread_counts; i++) {
			std::thread* th=new std::thread([&]() {
				http::client client(getws());
				client.init(
					[](websocketpp::connection_hdl hdl)
				{
					std::cout << "on open " << std::endl;
				},
					[&](websocketpp::connection_hdl hdl, http::message_ptr msg)
				{
					
					result_body body = fc::json::from_string(msg->get_payload()).as<result_body>();
					mytask[body.id].end_time = fc::time_point::now().sec_since_epoch();
					if (body.error.valid())
					{
						update(body.id, task_result::fail);
						std::cout << "on recv " << msg->get_payload() << std::endl;
					}
					else
						update(body.id);

				},
					[](websocketpp::connection_hdl hdl)
				{
					std::cout << "on close " << std::endl;
				},
					[](websocketpp::connection_hdl hdl)
				{
					std::cout << "on fail " << std::endl;

				});
				auto ff = factory::helper(); 
				while(true) 
				{
					if (isstop)
					{
						client.stop();
						break;
					}
					
					std::random_device rd;
					std::default_random_engine engine(rd());
					std::uniform_int_distribution<> dis(0, myusers_count-1);
					auto  productor = std::bind(dis, engine);
					uint64_t from = productor(); 
					auto give = myusers.lower_bound(from);
					uint64_t to = productor();
					auto giveto= myusers.lower_bound(to);
					if (give == giveto)
						continue;
					auto money=asset(NEWS_SYMBOL, (productor()+1)*1L);
					auto str = ff.create_transfer(give->second, give->first, giveto->first, money);
					uint64_t id = taskid++;
					std::string ret = string_json_rpc(id,fc::json::to_string(str));
					client.send_message(ret); 
					taskresult rs;
					rs.start_time = fc::time_point::now().sec_since_epoch();
					rs.id = id;
					rs.result = task_result::recvfail;
					rs.op = option_user::transfer_one;
					mytask.insert(std::make_pair(id, std::move(rs)));
				} 
			});
			th_pool.push_back(th);
		}
		return true;

	}
	  
	void show_result()
	{
		uint64_t ok = 0;
		uint64_t fail = 0;
		uint64_t responsetime = 0;
		uint64_t responsetime_network_ok = 0;
		uint64_t failnetwork = 0;
		ilog("time:${t}", ("t", endtime - starttime));
		for (auto it : mytask)
		{
			if (it.second.result == task_result::suces)
			{
				responsetime += (it.second.end_time - it.second.start_time);
				ok++;
			}
			else {
				fail++;
				if (it.second.result >= 0x1 && it.second.result <= 0x3)			
					failnetwork++;			
				else
					responsetime_network_ok += it.second.end_time - it.second.start_time;
			}
		}

		ilog("sucess:${t}", ("t",ok));
		ilog("fail:${t}", ("t", fail));
		ilog("fail-network:${t}", ("t", failnetwork));
		ilog("fail-but-network is ok:${t}", ("t", fail-failnetwork));
		ilog("ok tips/s:${t}", ("t", responsetime / ok));
		ilog("fail but network is ok :${t}", ("t", responsetime_network_ok / fail - failnetwork)); 
	 


	}
	bool end_business()
	{ 
		isstop = true;
		for (auto it : th_pool)
		{
			it->join();
			delete it;
		}
		endtime = fc::time_point::now().sec_since_epoch(); 
		return true;
	}
private:
	thread_pool th_pool; 
	int thread_counts;
	bool isstop;
	std::vector<std::string> wsaddress;
	
	users myusers; 
	int myusers_count; 
	std::mutex mutex_;


	uint64_t starttime;
	uint64_t endtime;
	tasklist mytask;
	std::atomic<std::uint64_t> taskid;  

	vector<string> endpoint;
	run_type mode;


	int wscount;
	int wsindex; 
	
}; 

int main(int argc, char **argv){
 
		program_options::options_description opts("options");
		opts.add_options()
			("threadpool", program_options::value<std::string>(), "threads count default 8")
			("ws", program_options::value<std::string>(), "set ws address for example (--ws=\"192.168.0.1:123456-192.168.2.2:7984\")")
			("mode", program_options::value<std::string>(), "send-ws-mode(include primary and balance,default balance)");
		program_options::variables_map vm;
		program_options::store(program_options::parse_command_line(argc, argv, opts), vm);

		cout << opts << endl;
		run_type type = run_type::balance;
		if (vm.count("mode")) {
			std::string runmode = vm["mode"].as<string>();
			if (runmode.compare("balance") && runmode.compare("primary"))
			{
				std::cout << "error startline of mode";
				return -1;
			}
		}

		int threadcount = 8;
		if (vm.count("threadpool")) {
			std::string wsaddress = vm["threadpool"].as<string>();
			threadcount = lexical_cast<int>(wsaddress.c_str());
		}

		vector<string> addresses;
		if (vm.count("ws")) {
			std::string wsaddress = vm["ws"].as<string>();
			boost::split(addresses, wsaddress, boost::is_any_of("-"));
		}
		else
		{
			std::cout << "lost startline of ws";
			getchar();
			return -1;
		}
		vector<string> wsaddr;
		for (auto it : addresses)
		{
			std::string str = "ws://";
			str.append(it.c_str());
			wsaddr.push_back(str);
		}
		business bs(wsaddr, type, threadcount);

		std::cout << "input: 1: gen account;2:transfer_one;3:transfer_ones" << std::endl;

		int i = -1;
		std::cin >> i;
		std::cout << "input x to stop the programe";
		switch (i)
		{
		case 1:
			bs.gen_account();
			break;
		case 2:
			bs.start_business();
			break;
		case 3:
			bs.start_business(true);
			break;
		}
		while (true)
		{
			if (getchar() != 'x')
				continue;
			bs.end_business();
				break;

		}
		bs.show_result();

		getchar();
	 
    return 0;
}


