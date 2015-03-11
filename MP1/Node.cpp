/*
 * server.cpp
 *
 *  Created on: Mar 8, 2015
 *      Author: moontails
 */

#include "headers/SocketBroker.h"
#include "headers/ConfigReader.h"
#include <iostream>
#include <string>
#include <thread>
#include <fstream>
#include <map>
#include <ctime>

using namespace std;

void time_printer()
{
  time_t t = time(0);   // get time now
  struct tm * now = localtime( & t );
  std::cout << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-'<<  now->tm_mday << ':' << ':';
  std::cout << now->tm_hour << '-' << (now->tm_min) << '-'<<  now->tm_sec<< endl;
}

void client(int portno, std::string hostname)
{
  /*
  std::string inputMessage;

  while(1)
  {
    std::cout << "\nWaiting for user input" << std::endl;
    std::getline( std::cin, inputMessage);
    std::cout << "User Input: " << inputMessage << "\n";
  }
  */
  ClientSocket client;

  while(1)
  {
    client.create();
    client.connect(portno, hostname);

    client.write();
    client.read();
  }

}

void server(int portno)
{
  /*
  std::string inputMessage;

  while(1)
  {
    std::cout << "Waiting for input connections: " << std::endl;
    std::getline( std::cin, inputMessage);
    std::cout << "User Input: " << inputMessage << "\n";
  }
  */

  ServerSocket server;
  ServerSocket newserver;

  server.create();
  server.bind(portno);

  server.listen();
  while(1)
  {
  server.accept(newserver);

  newserver.read();
  newserver.write();
  newserver.close();
  }
  server.close();
}

int main(int argc, char *argv[])
{
  if(argc < 3)
  {
    std::cout << "Usage: Nodename (A or B or C or D) Hostname " << std::endl;
    exit(0);
  }
  std::string nodeName = argv[1];
  std::string hostName = argv[2];
  ConfigReader *myconfig = new ConfigReader();
  int portno, delay;


  // display the nodinfo
  myconfig->readConfig("config.txt");
  portno = myconfig->nodeInfo[nodeName].first;
  delay = myconfig->nodeInfo[nodeName].second;

  std::cout << "\nNodeInfo:  NodeName : Port no : Delay (s) " << std::endl;
  std::cout << "\t\t" << nodeName << "\t" << portno << "\t" << delay << "\n\n";

  std::thread server_thread (server, portno);
  std::thread client_thread (client, portno, hostName);

  server_thread.join();
  client_thread.join();

}
