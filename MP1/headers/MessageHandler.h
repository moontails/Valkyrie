/*
 * MessageHandler.h
 *
 *  Created on: Mar 8, 2015
 *      Author: moontails, emch2
 */

#ifndef MESSAGEHANDLER_H_
#define MESSAGEHANDLER_H_

#include <iostream>
#include <string.h>
#include <vector>

#define DELIM ':'

enum operationType { Delete, Get, Insert, Update };

using namespace std;

class MessageHandler
{
public:
	operationType opType;
	static std::string serialize(std::string inputMessage);
	static std::vector<std::string> deserialize(std::string inputMessage);
	static std::string deserializeB(std::string inputMessage);
	static std::string serialize_map(std::map<int, std::pair<int,std::chrono::system_clock::time_point>> map);
	static std::map<int, std::pair<int,std::chrono::system_clock::time_point>> deserialize_map(std::string s);

};


#endif /* MESSAGEHANDLER_H_ */
