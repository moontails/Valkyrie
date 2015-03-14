/*
 * Storage.h
 * emch2
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

 class Storage{

 	
 	
 	public:

 		std::map<int, std::pair<int, std::chrono::system_clock::time_point>> key_value;

 		//i dont think these need models??
 		std::void delete(int key);
 		std::int get(int key);
 		std::void insert(int key, int value);
 		std:void update(int key, int value);

 		//possibly higher up i think
 		//std::void repair();
 		std::void show_all();
 		//std::void search(int key);


 };

 #endif
