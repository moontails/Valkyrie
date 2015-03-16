/*
* Storage.h
*
*  Created on: Mar 14, 2015
*      Author: emch2
*/


#ifndef STORAGE_H_
#define STORAGE_H_

#include <iostream>
#include <string.h>
#include <chrono>
#include <ctime>
#include <map>

using namespace std;
using std::chrono::system_clock;

class Storage
{

public:

  std::map<int, std::pair<int, std::chrono::system_clock::time_point>> key_value;

  void deleter(int);
  int getter(int);
  void inserter(int, int);
  void updater(int, int);
  void ec_updater(int, int, std::chrono::system_clock::time_point);
  void ec_inserter(int, int, std::chrono::system_clock::time_point);
  void show_all();
};

#endif  /* STORAGE_H_ */
