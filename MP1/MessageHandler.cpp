#include "headers/MessageHandler.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <map>
#include <vector>


using namespace std;

std::string MessageHandler::serialize(std::string inputMessage)
{
	std::string message = inputMessage;

	std::replace(message.begin(), message.end(), ' ', DELIM);

	return message;
}

std::string MessageHandler::deserializeB(std::string inputMessage)
{
	int pos1 = inputMessage.find(':');
	int pos2 = inputMessage.find_last_of(':');
	std::string message = inputMessage.substr(pos1+1, pos2-pos1-1);
	std::cout << message << std::endl;
	std::string nodeName = inputMessage.substr(pos2 + 1);
	return nodeName;
}

std::vector<std::string> MessageHandler::deserialize(std::string inputMessage)
{
	std::string token;
	std::vector<std::string> command;

	size_t pos = 0;
	pos = 0;
	while((pos = inputMessage.find(DELIM)) != std::string::npos){
		token = inputMessage.substr(0,pos);
		command.push_back(token);
		inputMessage.erase(0,pos+1);
	}

	command.push_back(inputMessage);
	return command;
}

/*
std::void classify(string [] array)
{
	//more testing
	cout<<destination<<endl;

	if(command[0].compare("delete")==0){
		deleteCommand(command[1]);
	}
	if(command[0].compare("get") == 0){
		getCommand(command[1], command[2]);
	}
	if(command[0].compare("insert") == 0){
		insertCommand(command[1],command[2], command[3]);
	}
	if(command[0].compare("update")==0){
		updateCommand(command[1], command[2], command[3]);
	}
	if(command[0].compare("Send") == 0){
		sendCommand(command[1], destination);
	}


	return;

}
*/
