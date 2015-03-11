/*
 * MessageHandler.h
 *
 *  Created on: Mar 8, 2015
 *      Author: moontails
 */

#ifndef MESSAGEHANDLER_H_
#define MESSAGEHANDLER_H_

#include <iostream>
#include <string.h>

#define DELIMITER ':'

enum operationType { Delete, Get, Insert, Update };

using namespace std;

class MessageHandler
{
public:
	operationType opType;
	static std::string serialize(std::string inputMessage);
	static std::string deserialize(std::string inputMessage);

};


#endif /* MESSAGEHANDLER_H_ */
