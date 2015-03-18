/*
* MessageHandler.h
*
*  Created on: Mar 8, 2015
*      Author: moontails
*/

#ifndef CONFIGREADER_H_
#define CONFIGRREADER_H_

#include <iostream>
#include <string>
#include <fstream>
#include <map>

#define DELIMITER '-'

using namespace std;

class ConfigReader
{
public:
  std::map<std::string, std::pair <int, int>> nodeInfo;
  void readConfig(const std::string filename);

};


#endif /* CONFIGREADER_H_ */
