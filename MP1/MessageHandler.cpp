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

	std::replace(message.begin(), message.end(), ' ', DELIM);

	return message;
}

std::string MessageHandler::deserialize(std::string inputMessage)
{
	std::map<std::string, std::string> result ;// = MessageHandler::deserialize(inputMessage);
	int pos1 = inputMessage.find(':');
	std::string operation = inputMessage.substr(0,pos1);
	result["operation"] = operation;
	//int pos2 = inputMessage.find_last_of(':');
	if(operation == "Send")
	{
		int pos2 = inputMessage.find(':', pos1+1);
		std::string message = inputMessage.substr(pos1+1, pos2-pos1-1);
		result["message"] = message;

		int pos3 = inputMessage.find(':', pos2+1);
		std::string dstName = inputMessage.substr(pos2+1,1);
		result["dstName"] = dstName;

		int pos4 = inputMessage.find(':', pos3+1);
		std::string srcName = inputMessage.substr(pos3+1,1);
		result["srcName"] = srcName;

	}
	else if(operation == "Delete")
	{
		int pos2 = inputMessage.find(':', pos1+1);
		std::string inputKey = inputMessage.substr(pos1+1, inputMessage.length()-pos2-1);
		result["key"] = inputKey;
	}
	else if(operation == "Insert")
	{
		int pos2 = inputMessage.find(':', pos1+1);
		int pos3 = inputMessage.find(':', pos2+1);
		std::string inputKey = inputMessage.substr(pos1+1, pos2-pos1-1);
		std::string inputVal = inputMessage.substr(pos2+1, pos3-pos2-1);
		std::string model = inputMessage.substr(pos3+1, 1);
		result["key"] = inputKey;
		result["val"] = inputVal;
		result["model"] = model;
	}
	else if(operation == "Get")
	{
		int pos2 = inputMessage.find(':', pos1+1);
		std::string inputKey = inputMessage.substr(pos1+1, pos2-pos1-1);
		std::string model = inputMessage.substr(pos2+1, 1);
		result["key"] = inputKey;
		result["model"] = model;
	}
	else if(operation == "Update")
	{
		int pos2 = inputMessage.find(':', pos1+1);
		int pos3 = inputMessage.find(':', pos2+1);
		int pos4 = inputMessage.find(':', pos3+1);
		std::string inputKey = inputMessage.substr(pos2+1, pos3-pos2-1);
		std::string inputVal = inputMessage.substr(pos3+1, pos4-pos3-1);
		std::string model = inputMessage.substr(pos4+1, 1);
		result["key"] = inputKey;
		result["val"] = inputVal;
		result["model"] = model;
	}
	return result["operation"];
}
