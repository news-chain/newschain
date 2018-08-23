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
#include <chainbase\chainbase.hpp>
  
using namespace chainbase;
using namespace std;
int main(int argc, char **argv){

	std::cout << "read account_object_index" << std::endl; 
	unique_ptr<bip::managed_mapped_file>                        _segment;

	char abs_path[256] = { 0 };
	strcpy(abs_path, argv[1]);
	if (!boost::filesystem::exists(abs_path))
	{
		std::cout << abs_path << " is not exists" << std::endl;
		return 2;
	}
	_segment.reset(new bip::managed_mapped_file(bip::open_only,
		abs_path)); 
	typedef generic_index<news::base::account_object_index> index_type;
	typedef index_type*                   index_type_ptr; 
	 auto idx=_segment->find<index_type>("class news::base::account_object"); 
 
	const auto& indices = idx.first->indices().get<news::base::by_id>();

	for (auto it= indices.begin(); it!= indices.end();it++)
	{
		news::base::account_object value = *it;
		std::cout << "id:"<< value.id <<" name: " <<value.name << std::endl;
	}
	while (getchar() != 'x')
		;
	 
    return 0;
}


