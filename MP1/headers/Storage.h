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

  //i dont think these need models??
  void deleter(int);
  int getter(int);
  void inserter(int, int);
  void updater(int, int);

  //possibly higher up i think
  //std::void repair();
  void show_all();
  //std::void search(int key);
};

#endif  /* STORAGE_H_ */
