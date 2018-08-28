//
// Created by boy on 18-6-20.
//
#include <stdio.h>
#include<iostream>
#include <memory>
#include <map>
#include <news/base/account_object.hpp> 
#include <news/base/comment_object.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/set.hpp>
#include <boost/interprocess/containers/flat_map.hpp>
#include <boost/interprocess/containers/deque.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/interprocess_sharable_mutex.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include <chainbase/chainbase.hpp>
 
#include "mysql/mysql.h"

using namespace chainbase;
using namespace std; 
const char* user = "root";
const char* pswd = "root";
const char* host = "192.168.0.198";
unsigned int port = 3306; 
MYSQL myCont;
bool hasinit = false;
int64_t countsok = 0;


bool init_mysql()
{
	mysql_init(&myCont);
	if (!mysql_real_connect(&myCont, host, user, pswd, "news", port, NULL, 0))
	{
		std::cout << mysql_error(&myCont) << std::endl;
		return false;
	}
	return hasinit = true;
}
void fater_mysql()
{
	if (hasinit)
		mysql_close(&myCont);
}
void excute_sql(std::string sql)
{
	int ret = mysql_query(&myCont, sql.c_str());
	if (ret)
	{
		std::cout << sql.c_str() << " fail" << std::endl;
	}
	countsok++;
	if (countsok % 100 == 0)
		std::cout << "has insert " << countsok << "lines" << std::endl;

}


/*
class db_operator
{ 
  public:  
	  virtual ~db_operator() {};
	virtual int read_from_mmp() = 0; 
	virtual int insert_db() = 0;
  private:

};
*/

template<typename MultiIndexType, typename obj>
class db_account_object 
{
public:	
	typedef generic_index<MultiIndexType> index_type;
	typedef obj my_obj;
	db_account_object(std::shared_ptr<bip::managed_mapped_file> segment,std::string table_name) :_segment(segment), _table_name(table_name)
	{ 
	    type_name = boost::core::demangle(typeid(typename index_type::value_type).name());
		std::cout << "start parse" << type_name.c_str();
	}
	virtual ~db_account_object()
	{ 
		std::cout << "end parse" << type_name.c_str();
	}  
	template<typename by_id>
	int read_from_mmp()
	{
		auto idx = _segment->find<index_type>(type_name.c_str()); 	
		const auto& indices = idx.first->indices().get<by_id>();
		for (auto it = indices.begin(); it != indices.end(); it++)
		{
			dblist.emplace_back(*it); 
		}
		return 0;

	}
	//template<typename obj>
	int insert_db()
	{
		std::vector<std::string> table_clom;
		for (auto it : dblist)
		{
			fc::variant_object oo = fc::variant(it).as<fc::variant_object>();
			for (auto itr = oo.begin(); itr != oo.end(); itr++) {  
					table_clom.emplace_back(itr->key()); 
			}
			break; 
		} 
		for ( auto it : dblist)
		{
			std::vector<std::string> values;
			auto oo=fc::variant(it).as<fc::variant_object>();
			for (auto itr = oo.begin(); itr != oo.end(); itr++) { 
				values.emplace_back(itr->value().as_string());
			}
			char buff[2048] = { 0 };
			char buff_format[2048] = { 0 };
			std::string sql = " insert into  " + _table_name + "(";
			std::string valuesstr = "values(";
			int pos = 0;
			for (auto i : table_clom)
			{
				pos++;
				sql.append(i);// .append(""); 
				
				if (pos == table_clom.size())
				{				
					valuesstr.append("\"%s\"");
				}
				else
				{
					sql.append(",");
					valuesstr.append("\"%s\",");
				}
					
			}
			sql.append(")"); 
			valuesstr.append(")");
			sql.append(valuesstr);
			strcpy(buff, sql.c_str());
			int size = values.size();
			switch (size)
			{
			case 4:
				sprintf(buff_format, buff, values[0].c_str(), values[1].c_str(), values[2].c_str(), values[3].c_str()); break;
			case 5:
				sprintf(buff_format, buff, values[0].c_str(), values[1].c_str(), values[2].c_str(), values[3].c_str(), values[4].c_str()); break;
			case 6:
				sprintf(buff_format, buff, values[0].c_str(), values[1].c_str(), values[2].c_str(), values[3].c_str(), values[4].c_str(), values[5].c_str()); break;
			case 7:
				sprintf(buff_format, buff, values[0].c_str(), values[1].c_str(), values[2].c_str(), values[3].c_str(), values[4].c_str(), values[5].c_str(), values[6].c_str()); break;
			case 8:
				sprintf(buff_format, buff, values[0].c_str(), values[1].c_str(), values[2].c_str(), values[3].c_str(), values[4].c_str(), values[5].c_str(), values[6].c_str(), values[7].c_str()); break;
			case 9:
				sprintf(buff_format, buff, values[0].c_str(), values[1].c_str(), values[2].c_str(), values[3].c_str(), values[4].c_str(), values[5].c_str(), values[6].c_str(), values[7].c_str(), values[8].c_str()); break;
			case 10:
				sprintf(buff_format, buff, values[0].c_str(), values[1].c_str(), values[2].c_str(), values[3].c_str(), values[4].c_str(), values[5].c_str(), values[6].c_str(), values[7].c_str(), values[8].c_str(), values[9].c_str()); break;
			default:
				break;
			}
			excute_sql(std::string(buff_format));

				
		}
		return 0;
	}

private:
	std::shared_ptr<bip::managed_mapped_file>  _segment; 
	std::string type_name;
	std::vector<obj> dblist;
	std::string _table_name;
};

void example(std::string argv)
{

	std::cout << "read account_object_index" << std::endl;
	unique_ptr<bip::managed_mapped_file>                        _segment;

	char abs_path[256] = { 0 };
	strcpy(abs_path, argv.c_str());
	if (!boost::filesystem::exists(abs_path))
	{
		std::cout << abs_path << " is not exists" << std::endl;
		return ;
	}
	_segment.reset(new bip::managed_mapped_file(bip::open_only,
		abs_path));
	typedef generic_index<news::base::account_object_index> index_type;
	typedef index_type*                   index_type_ptr;
	auto idx = _segment->find<index_type>("class news::base::account_object");

	const auto& indices = idx.first->indices().get<news::base::by_id>();

	for (auto it = indices.begin(); it != indices.end(); it++)
	{
		news::base::account_object value = *it;
		std::cout << "id:" << value.id << " name: " << value.name << std::endl;
	}
	while (getchar() != 'x')
		;
	_segment.reset();
}

/*
template<typename MultiIndexType, typename obj>
class my :public db_account_object<MultiIndexType, obj>
{
public:
	my(std::shared_ptr<bip::managed_mapped_file> segment) :db_account_object(segment)
	{}
	~my() {}
	 
};
*/




int main(int argc, char **argv){ 


	if (!init_mysql())
		return -1; 

	std::shared_ptr<bip::managed_mapped_file>              _segment; 
	std::string abs_path;
	//strcpy(abs_path, argv[1]);
    std::cin>>abs_path;
	if (!boost::filesystem::exists(abs_path))
	{
		std::cout << abs_path << " is not exists" << std::endl;
		return 0; 
	}
	_segment.reset(new bip::managed_mapped_file(bip::open_only,
		abs_path.c_str()));

	{
		db_account_object<news::base::account_object_index, news::base::account_object>  obj(_segment,"account_object");
		obj.read_from_mmp<news::base::by_id>();
		//obj.insert_db<news::base::account_object>();
		obj.insert_db();

	} 


	excute_sql("commit");
	fater_mysql();
 
    return 0;
}


