/*
 * server.cpp
 *
 *  Created on: Mar 15, 2015
 *      Author: moontails, emch2
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
std::queue<std::string> repairQ;
std::mutex mtx1;
std::mutex mtx2;

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
  if(inputCommand.substr(0,6)=="return")
  {
    mtx2.lock();
    repairQ.push(inputCommand);
    std::cout << "\nPushed to the repair queue on SERVER at " << ctime(&timestamp);
    mtx2.unlock();
  }
  else
  {
    time_point = std::chrono::system_clock::now();
    timestamp = std::chrono::system_clock::to_time_t(time_point);

    mtx1.lock();
    messageQ.push(inputCommand);
    std::cout << "\nPushed to the message queue on SERVER at " << ctime(&timestamp);
    std::cout << "\n Message is-" << inputCommand << std::endl;
    mtx1.unlock();
  }
  newserver.close();
  }
  server.close();
}

/* The inconsistency repair tool. Runs every 120 seconds. Ensures all nodes have a complete
*  replica of the key value store
*/

void map_repair()
{
  ClientSocket client;
  std::string hostname="localhost";

  while(1)
  {
    std::this_thread::sleep_for(std::chrono::seconds(300));

    for(std::map<std::string, std::pair<int,int>>::iterator it = myconfig->nodeInfo.begin(); it != myconfig->nodeInfo.end(); ++it)
    {
      client.create();
      client.connect(it->second.first, hostname);
      client.write("request");
      client.close();
    }


    while(1)
    {
      if(repairQ.size()==4)
        break;
    }

    mtx2.lock();
    std::string string_a = repairQ.front().erase(0,6);
    repairQ.pop();
    std::string string_b = repairQ.front().erase(0,6);
    repairQ.pop();
    std::string string_c = repairQ.front().erase(0,6);
    repairQ.pop();
    std::string string_d = repairQ.front().erase(0,6);
    repairQ.pop();
    mtx2.unlock();

    std::map<int, std::pair<int, std::chrono::system_clock::time_point>> map_a = MessageHandler::deserialize_map(string_a);
    std::map<int, std::pair<int, std::chrono::system_clock::time_point>> map_b = MessageHandler::deserialize_map(string_b);
    std::map<int, std::pair<int, std::chrono::system_clock::time_point>> map_c = MessageHandler::deserialize_map(string_c);
    std::map<int, std::pair<int, std::chrono::system_clock::time_point>> map_d = MessageHandler::deserialize_map(string_d);

    std::map<int, std::pair<int, std::chrono::system_clock::time_point>> map_all = map_a;

    for(std::map<int, std::pair<int, std::chrono::system_clock::time_point>>::iterator it = map_b.begin(); it != map_b.end(); ++it)
    {
      if(map_all.find(it->first) != map_all.end())
      {
        if(it->second.second > map_all.find(it->first)->second.second)
        {
          map_all[it->first] = it->second;
        }
      }
      else
      {
        map_all[it->first] = it->second;
      }
    }

    for(std::map<int, std::pair<int, std::chrono::system_clock::time_point>>::iterator it = map_c.begin(); it != map_c.end(); ++it)
    {
      if(map_all.find(it->first) != map_all.end())
      {
        if(it->second.second > map_all.find(it->first)->second.second)
        {
          map_all[it->first] = it->second;
        }
      }
      else
      {
        map_all[it->first] = it->second;
      }
    }

    for(std::map<int, std::pair<int, std::chrono::system_clock::time_point>>::iterator it = map_d.begin(); it != map_d.end(); ++it)
    {
      if(map_all.find(it->first) != map_all.end())
      {
        if(it->second.second > map_all.find(it->first)->second.second)
        {
          map_all[it->first] = it->second;
        }
      }
      else{
        map_all[it->first] = it->second;
      }
    }

    std::string serialized_map = "repair";
    serialized_map.append(MessageHandler::serialize_map(map_all));

    for(std::map<std::string, std::pair<int,int>>::iterator it = myconfig->nodeInfo.begin(); it != myconfig->nodeInfo.end(); ++it){
      client.create();
      client.connect(it->second.first, hostname);
      client.write(serialized_map);
      client.close();
    }

  }

}

int main(int argc, char *argv[])
{
  myconfig->readConfig("config.txt");

  std::thread server_thread (server);
  std::thread client_thread (client);
  std::thread repair_thread (map_repair);

  server_thread.join();
  client_thread.join();
  repair_thread.join();

  return 0;
}
