/*
 * MessageHandler.cpp
 *
 *  Created on: Mar 8, 2015
 *      Author: moontails
 */

#include "headers/MessageHandler.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <map>

using namespace std;

std::string MessageHandler::serialize(std::string inputMessage)
{
	std::string message = inputMessage;

	std::replace(message.begin(), message.end(), ' ', DELIMITER);

	return message;
}

std::string MessageHandler::deserialize(std::string inputMessage)
{
	int pos1 = inputMessage.find(':');
	int pos2 = inputMessage.find_last_of(':');
	std::string message = inputMessage.substr(pos1+1, pos2-pos1-1);
	std::cout << message << std::endl;
	std::string nodeName = inputMessage.substr(pos2 + 1);
	return nodeName;
}
