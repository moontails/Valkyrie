/*
 * Storage.cpp
 * emch2
 */

 #include "headers/Storage.h"
 #include <iostream>
 #include <algorithm>
 #include <string>
 #include <map>

 using namespace std;
 using std::chrono::system_clock; 

 std::void Storage::delete(int key)
 {

 	std::cout<<"Deleted key: "<<key<<" and its value: "<<key_value.find(key)->second.first<<std::endl;
 	key_value.erase(key);
 	
 }

 std::int Storage::get(int key, int model)
 {
 	std::cout<<"key: "<<key<<", value: "<<key_value.find(key)->second.first<<std::endl;
 	return key_value.find(key)->second.first; //?

 }

 std::void Storage::insert(int key, int value)
 {
 	
 	system_clock::time_point now = system_clock::now();

 	std::pair<int, std::chrono::system_clock::time_point> value_time (value, now); 
 	key_value.insert(std::pair<int, std::pair<int, std::chrono::system_clock::time_point>>(key, value_time));

 	std::time_t t = system_clock::to_time_t(key_value.find(key)->second.second);
 	std::cout<<"inserted key: "<<key<<", with value: "<<key_value.find(key)->second.first<<", at clock time: "<<ctime(&t)<<std::endl;

 }

 std::void Storage::update(int key, int value)
 {
 	system_clock::time_point now = system_clock::now();
 	std::pair<int, std::chrono::system_clock::time_point> new_value_time (value, now);
 	key_value[key] = new_value_time;
 	std::time_t t = system_clock::to_time_t(key_value.find(key)->second.second);
 	std::cout<<"Upated key: "<<key<<", with new value: "<<key_value.find(key)->second.first<<", at time"<<ctime(&t)<<std::endl;
 }

 std::void show_all()
 {
 	std::map<int, std::pair<int, std::chrono::system_clock::time_point>>::iterator it;
 	std::time_t t;
 	for(it = key_value.begin(); it != key_value.end(); ++it){
 		t = system_clock::to_time_t(it->second.second);
 		std::cout<<"Key: "<<it->first<<", Value: "<<it->second.first<<", Timestamp: "<<ctime(&t)<<std::endl;
 	}
 }
 

