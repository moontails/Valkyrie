/*
* Storage.cpp
*
*  Created on: Mar 14, 2015
*      Author: emch2
*/

#include "headers/Storage.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <map>

using namespace std;
using std::chrono::system_clock;

void Storage::deleter(int key)
{
  key_value.erase(key);
  std::cout<<"Key "<<key<<" deleted"<<std::endl;
}

int Storage::getter(int key)
{
  auto it = key_value.find(key);

  if(it == key_value.end())
  {
    std::cout<<"Key "<<key<<" not found :("<<std::endl;
    return -1;
  }

  std::cout<<"key: "<<key<<", value: "<<key_value.find(key)->second.first<<std::endl;
  return key_value.find(key)->second.first; //?

}

void Storage::inserter(int key, int value)
{

  system_clock::time_point now = system_clock::now();

  std::pair<int, std::chrono::system_clock::time_point> value_time (value, now);
  key_value.insert(std::pair<int, std::pair<int, std::chrono::system_clock::time_point>>(key, value_time));

  std::time_t t = system_clock::to_time_t(key_value.find(key)->second.second);
  std::cout<<"Inserted key "<<key<<std::endl;

}

void Storage::updater(int key, int value)
{
  int old_value = key_value.find(key)->second.first;
  system_clock::time_point now = system_clock::now();
  std::pair<int, std::chrono::system_clock::time_point> new_value_time (value, now);

  key_value[key] = new_value_time;

  std::time_t t = system_clock::to_time_t(key_value.find(key)->second.second);
  std::cout<<"Key "<<key<<" changed from "<<old_value<<" to "<<key_value.find(key)->second.first<<std::endl;
}

void Storage::show_all()
{
  std::map<int, std::pair<int, std::chrono::system_clock::time_point>>::iterator it;
  std::time_t t;

  for(it = key_value.begin(); it != key_value.end(); ++it)
  {
    t = system_clock::to_time_t(it->second.second);
    std::cout<<"Key: "<<it->first<<", Value: "<<it->second.first<<", Timestamp: "<<ctime(&t)<<std::endl;
  }
}

void Storage::ec_updater(int key, int value, std::chrono::system_clock::time_point ts)
{
	int old_value = key_value.find(key)->second.first;
	if(ts > key_value.find(key)->second.second){
		std::pair<int, std::chrono::system_clock::time_point> new_value_time (value, ts);
		key_value[key] = new_value_time;
		std::cout<<"Key "<<key<<" changed from "<<old_value<<" to "<<key_value.find(key)->second.first<<std::endl;
	}
	else{
		std::cout<<"New update already stored. Key not updated"<<std::endl;
	}
}

void Storage::ec_inserter(int key, int value, std::chrono::system_clock::time_point ts)
{
  if(ts>key_value.find(key)->second.second){
    std::pair<int , std::chrono::system_clock::time_point> new_value_time (value, ts);
    key_value.insert(std::pair<int, std::pair<int, std::chrono::system_clock::time_point>>(key, new_value_time));
    std::cout<<"Inserted key "<<key<<std::endl; 
  }
  else{
    std::cout<<"New version already insterted. Key value not inserted"<<std::endl;
  }
}