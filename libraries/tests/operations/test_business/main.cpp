//
// Created by boy on 18-6-20.
//
#include<thread>
#include <fc/exception/exception.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp> 
#include <iostream>
#include <wsClient.hpp>
#include <fc/io/json.hpp>
#include "factory.hpp"


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
	fail = recvfail + 1,
	suces= fail+1
};
struct taskresult
{
	uint64_t id;
	option_user op;
	task_result result;
	uint64_t username;
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
	business(std::vector<std::string> &ws_, run_type ty= run_type::balance, int thread_count_=8,uint64_t userstartid=1000) :wsaddress(ws_), mode(ty),thread_counts(thread_count_),isstop(false), startid(userstartid)
	{ 
		startid_first = userstartid;
		myusers.insert(std::make_pair(1, NEWS_INIT_PRIVATE_KEY));
		myusers_name.push_back(1);
		wscount = wsaddress.size();
		taskid = 100;
		wsindex= 0; 
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
		{
			std::lock_guard<std::mutex> lock(mutex_mytask);
			mytask[id].end_time = fc::time_point::now().time_since_epoch().count();
			mytask[id].result = fs;
			{
				if (mytask[id].op == option_user::create_user&&fs== task_result::suces)
				{
					std::lock_guard<std::mutex> lock(mutex_myusers_name);
					myusers_name.push_back(mytask[id].username);
				}

			}
		}
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
			if (body.error.valid())
			{
				std::cout << msg->get_payload() << std::endl;
				update(body.id, task_result::fail);
			}
			else
			{
				update(body.id);
			}

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
	 
		for (int i=0; i <accounts;i++)
		{  
			startid++;
			news::base::private_key_type genkey;  
			auto str = ff.create_account(myusers[1], 1, startid, genkey); 
			std::string ret = string_json_rpc(++taskid, fc::json::to_string(str));
			client.send_message(ret);
			myusers[startid] = genkey; 
			
			uint64_t id = ++taskid;
			auto money = asset(NEWS_SYMBOL, 300 * 10000L);
			ff.create_transfer(id,myusers[1],1, startid, money);
			ret = string_json_rpc(id, fc::json::to_string(str));
			client.send_message(ret);

		} 
		client.stop();
		return true;

	}
	bool gen_account()
	{ 
			assert(thread_counts>0); 
			starttime = fc::time_point::now().time_since_epoch().count();
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
						result_body body = fc::json::from_string(msg->get_payload()).as<result_body>(); 						
						if (body.error.valid())
						{
							std::cout << msg->get_payload() << std::endl;
							update(body.id, task_result::fail);
						}
						else
						{
							update(body.id);
						}
						 
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
					uint64_t lastlogtime = 0;
					std::random_device rd;
					std::default_random_engine engine(rd());
					while (true)
					{
						if (isstop)
						{
							client.stop();
							break;
						} 
						int users_name_size = 0;
						{
							std::lock_guard<std::mutex> lock(mutex_myusers_name);
							users_name_size=myusers_name.size();
						}
						std::uniform_int_distribution<> dis(0, users_name_size-1);
						auto  productor = std::bind(dis, engine);
						uint64_t from =  productor();
						uint64_t genuser = startid++;						
						news::base::private_key_type genkey;
						auto admin = myusers.at(myusers_name[from]);
						auto str = ff.create_account(admin, myusers_name[from],genuser, genkey);
						auto id = taskid++; 
						std::string ret = string_json_rpc(id,fc::json::to_string(str));
						client.send_message(ret); 
						taskresult rs;
						auto times= fc::time_point::now().time_since_epoch().count(); 
						if ((times - lastlogtime) > 1000000 * 10)
						{
							lastlogtime = times;
							std::cout << "has send " << taskid << " message " << std::endl;
						}
						rs.start_time = times;
						rs.id = id;
						rs.result = task_result::recvfail;
						rs.username = genuser;
						rs.op = option_user::create_user;
						{ 
							std::lock_guard<std::mutex> lock(mutex_mytask);
							mytask.insert(std::make_pair(id, std::move(rs)));
						}						
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
		//std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		boost::this_thread::sleep(boost::posix_time::seconds(5));
		std::cout << "have " << myusers_name.size() << " account to test the transfer" << std::endl;
		if (myusers_name.size() <= 5)
			return false;
		assert(thread_counts>0);
		starttime = fc::time_point::now().time_since_epoch().count();
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
				static uint64_t lastlogtime=0;
				std::random_device rd;
				std::default_random_engine engine(rd());
				while(true) 
				{
					if (isstop)
					{
						client.stop();
						break;
					}  
					std::uniform_int_distribution<> dis(0, myusers_name.size()-1);
					auto  productor = std::bind(dis, engine);
					uint64_t from = productor(); 
					uint64_t to = productor();
					if (from == to)
						continue; 
					auto money=asset(NEWS_SYMBOL, (productor()+1)*1L);
					uint64_t id = taskid++;
					auto str = ff.create_transfer(id, myusers[myusers_name[from]],myusers_name[from], myusers_name[to], money);
					std::string ret = string_json_rpc(id,fc::json::to_string(str));
					client.send_message(ret); 
					taskresult rs;
					auto times= fc::time_point::now().time_since_epoch().count();
					if ((times - lastlogtime) > 1000000 * 10)
					{
						lastlogtime = times;
						std::cout << "has send " << taskid << " message " << std::endl;
					}
						rs.start_time = times;
					rs.id = id;
					rs.result = task_result::recvfail;
					rs.op = option_user::transfer_one;
					{
						std::lock_guard<std::mutex> lock(mutex_mytask);
						mytask.insert(std::make_pair(id, std::move(rs)));
					}
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
		ilog("time:${t}", ("t", (endtime - starttime)/1000000));
		for (auto it : mytask)
		{
			if (it.second.result == task_result::suces)
			{
				responsetime += (it.second.end_time - it.second.start_time);
				ok++;
			}
			else {
				fail++;
				if (it.second.result <= 0x3)			
					failnetwork++;			
				else
					responsetime_network_ok += (it.second.end_time - it.second.start_time);
			}
		}

		ilog("sucess:${t}", ("t",ok));
		ilog("fail:${t}", ("t", fail));
		ilog("fail-network:${t}", ("t", failnetwork));
		ilog("fail-but-network is ok:${t}", ("t", fail-failnetwork));
	    float	rs = ok/((endtime - starttime) / 1000000);
		ilog("ok tips/s:${t}", ("t", rs));
		 rs = (fail - failnetwork) / ((endtime - starttime) / 1000000); 
		ilog("fail but network is ok tips/s:${t} ", ("t", rs));


	}
	bool end_business()
	{ 
		isstop = true;
		for (auto it : th_pool)
		{
			it->join();
			delete it;
		}
		endtime = fc::time_point::now().time_since_epoch().count();
		return true;
	}
private:
	thread_pool th_pool; 
	int thread_counts;
	bool isstop;
	std::vector<std::string> wsaddress;
	
	std::mutex mutex_myusers_name;
	std::vector<news::base::account_name> myusers_name; 
	users myusers;  
	std::mutex mutex_;


	uint64_t starttime;
	uint64_t endtime;
	tasklist mytask;
	std::mutex mutex_mytask;
	std::atomic<std::uint64_t> taskid;  

	vector<string> endpoint;
	run_type mode;

	uint64_t startid_first;
	std::atomic<std::uint64_t>  startid;

	int wscount;
	int wsindex; 
	
}; 

int main(int argc, char **argv){
 
		program_options::options_description opts("options");
		opts.add_options()
			("threadpool", program_options::value<std::string>(), "threads count default 8")
			("startid", program_options::value<std::string>(), "create user startid ,default 1000")
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
		int startid = 1000;
		if (vm.count("startid")) {
			std::string wsaddress = vm["startid"].as<string>();
			startid = lexical_cast<int>(wsaddress.c_str());
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
		business bs(wsaddr, type, threadcount,startid);

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


