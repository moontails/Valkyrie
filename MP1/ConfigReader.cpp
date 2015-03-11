/*
* SocketBrokerBroker.cpp
*
*  Created on: Feb 21, 2015
*      Author: moontails
*
*      Implementation of the SocketBrokerBroker class.
*/

#include "headers/ConfigReader.h"

using namespace std;

void ConfigReader::readConfig (const std::string filename)
{
  fstream configFile (filename.c_str());

  int pos1,pos2;
  std::string line, name;
  std::pair <int, int> temp;

  if (configFile.is_open())
  {
    while ( getline(configFile,line) )
    {
      name = line.substr(0,1);

      pos1 = line.find('-');
      pos2 = line.find('-',pos1+1);

      temp.first = std::stoi(line.substr(pos1+1,pos2));
      temp.second = std::stoi(line.substr(pos2+1));

      //std::cout << line << '\n';
      nodeInfo[name] = temp;
    }
    configFile.close();
  }
}
