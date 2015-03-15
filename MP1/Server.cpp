/*
 * server.cpp
 *
 *  Created on: Mar 15, 2015
 *      Author: moontails
 */

#include "headers/SocketBroker.h"
#include "headers/ConfigReader.h"
#include "headers/MessageHandler.h"

#include <thread>
#include <ctime>
#include <ratio>
#include <chrono>
#include <fstream>
#include <map>
#include <vector>
#include <queue>
#include <mutex>

#define SERVER 12350

using namespace std;

ConfigReader *myconfig = new ConfigReader();
std::queue<std::string> messageQ;
std::mutex mtx1;

// client thread to poll the message queue, and then broadcast it to all nodes
void client()
{
  ClientSocket client;
  std::string hostname = "localhost";

  while(1)
  {

    if(!messageQ.empty())
    {
      // acquire lock before deleting from message queue
      mtx1.lock();
      std::string inputMessage = messageQ.front();
      messageQ.pop();
      mtx1.unlock();
      std::cout<<"Sending-"<<inputMessage;
      for (std::map<std::string,std::pair<int,int>>::iterator it=myconfig->nodeInfo.begin(); it!=myconfig->nodeInfo.end(); ++it)
      {
        client.create();
        client.connect(it->second.first, hostname);

        client.write(inputMessage);
        client.close();
      }

    }

  }

}

void server()
{
  using std::chrono::system_clock;
  std::chrono::system_clock::time_point time_point;
  std::time_t timestamp;

  ServerSocket server;
  ServerSocket newserver;
  std::string inputCommand;


  server.create();
  server.bind(SERVER);

  server.listen();
  while(1)
  {
  server.accept(newserver);

  inputCommand = newserver.read();

  time_point = std::chrono::system_clock::now();
  timestamp = std::chrono::system_clock::to_time_t(time_point);

  mtx1.lock();
  messageQ.push(inputCommand);
  std::cout << "\nPushed to the message queue on SERVER at " << ctime(&timestamp);
  std::cout << "\n Message is-" << inputCommand << std::endl;
  mtx1.unlock();

  newserver.close();
  }
  server.close();
}

int main(int argc, char *argv[])
{
  myconfig->readConfig("config.txt");

  std::thread server_thread (server);
  std::thread client_thread (client);

  server_thread.join();
  client_thread.join();

  return 0;
}
