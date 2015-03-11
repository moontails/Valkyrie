/*
 * server.cpp
 *
 *  Created on: Mar 8, 2015
 *      Author: moontails
 */

#include "headers/SocketBroker.h"
#include "headers/ConfigReader.h"

#include <thread>
#include <fstream>
#include <map>
#include <ctime>

using namespace std;

void client(int portno, std::string hostname)
{
  ClientSocket client;
  std::string serverOutput;

  while(1)
  {
    client.create();
    client.connect(portno, hostname);

    client.write("test");
    serverOutput = client.read();
  }
  client.close();
}

void server(int portno)
{
  ServerSocket server;
  ServerSocket newserver;
  std::string inputCommand;

  server.create();
  server.bind(portno);

  server.listen();
  while(1)
  {
  server.accept(newserver);

  inputCommand = newserver.read();
  newserver.write(inputCommand);
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
