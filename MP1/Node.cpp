/*
 * server.cpp
 *
 *  Created on: Mar 8, 2015
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
ConfigReader *Nodes = new ConfigReader();
std::map<std::string, int> commandMap;
std::queue<std::pair <std::string, std::chrono::system_clock::time_point>> messageQ;
std::mutex mtx1;

// To initialize the other node information
void init_nodes(ConfigReader *result, const std::string fileName, const std::string nodeName)
{
  fstream configFile (fileName.c_str());

  int pos1,pos2;
  std::string line, name;
  std::pair <int, int> temp;

  if (configFile.is_open())
  {
    while ( getline(configFile,line) )
    {
      name = line.substr(0,1);

      if(name == nodeName)
        continue;

      pos1 = line.find('-');
      pos2 = line.find('-',pos1+1);

      temp.first = std::stoi(line.substr(pos1+1,pos2)); //portno
      temp.second = std::stoi(line.substr(pos2+1)); //delay

      //std::cout << line << '\n';
      result->nodeInfo[name] = temp;
    }
    configFile.close();
  }
}

void modelBasedBroadcast(std::string inputMessage, std::string hostname)
{
  ClientSocket client;

  std::vector<std::string> inputMessageVector = MessageHandler::deserialize(inputMessage);
  std::string command = inputMessageVector.front();

  // for commands get, insert, update, send to CentralServer for model 1 and 2
  if(command == "get" || command == "insert" || command == "update")
  {
    std::string modelNumber = inputMessageVector.back();
    if(modelNumber == "1" || modelNumber == "2")
    {
      client.create();
      client.connect(SERVER, hostname);

      client.write(inputMessage);
      client.close();
      return;
    }
  }

  // else just broadcast for other cases
  for (std::map<std::string,std::pair<int,int>>::iterator it=myconfig->nodeInfo.begin(); it!=Nodes->nodeInfo.end(); ++it)
  {
    client.create();
    client.connect(it->second.first, hostname);

    client.write(inputMessage);
    client.close();
  }

}
/*
// function to handle the differect consistency models
void consistencyModel(std::string inputMessage)
{
  std::vector<std::string> inputMessageVector = MessageHandler::deserialize(inputMessage);
  std::sting command = inputMessageVector.front();

  switch(commandMap[command])
  {
    case 1: // Send
      break;
    case 2: // delete
      break;
    case 3: // get
      break;
    case 4: // insert
      break;
    case 5: // update
      break;
    case 6: // show-all
      break;
    case 7: // search
      break;
    default: // invalid command
      std::cout << "\nInvalid Command, please enter one of {Send, delete, get, insert, update, show-all, search}" << std::endl;
      break;
  }

}
*/

// poller thread to poll the message queue, and check if the delay amount of time has passed.
void poller(int nodeDelay, std::string hostname)
{
  // to store time stamp variable
  std::chrono::system_clock::time_point stop, start;

  while(1)
  {

    stop = std::chrono::system_clock::now();

    if(!messageQ.empty())
    {
      std::pair <std::string, std::chrono::time_point<std::chrono::system_clock>> temp = messageQ.front();

      std::string inputMessage = temp.first;
      start = temp.second;
      // Deserialize the message to get nodename and
      //std::map<std::string, std::string> info = MessageHandler::deserialize(inputMessage);
      std::chrono::seconds delay = std::chrono::duration_cast<std::chrono::seconds>(stop - start);

      if(int(delay.count()) > nodeDelay)
      {
        mtx1.lock();
        messageQ.pop();
        mtx1.unlock();

        modelBasedBroadcast(inputMessage, hostname);
      }

    }

  }
}

// client thread to obain user input command, affix timestamp to the command and push it into queue.
void client(std::string hostName, std::string nodeName)
{
  using std::chrono::system_clock;
  //using std::chrono::duration_cast;

  std::pair <std::string, std::chrono::system_clock::time_point> temp;

  // to store time stamp variable
  std::chrono::system_clock::time_point time_point;
  // to store user input command
  std::string inputMessage;
  std::time_t timestamp;
  //ClientSocket client;

  while(1)
  {
    // Get the user input
    std::cout << "\nPlease enter the message: ";
    std::getline(std::cin, inputMessage);

    // seriliaze the message
    inputMessage = MessageHandler::serialize(inputMessage);// + ":" + nodeName;
    time_point = std::chrono::system_clock::now();
    timestamp = std::chrono::system_clock::to_time_t(time_point);

    temp.first = inputMessage;
    temp.second = time_point;

    mtx1.lock();
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    messageQ.push(temp);
    std::cout << "\nPushed to the message queue at " << ctime(&timestamp);
    mtx1.unlock();
    /* connect and send it to the server
    client.create();
    client.connect(SERVER, hostName);

    client.write(inputMessage);
    //serverOutput = client.read();
    client.close();
    */
  }
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
  //consistencyModel(inputCommand);
  //newserver.write(inputCommand);
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

  myconfig->readConfig("config.txt");

  std::string nodeName = argv[1];
  std::string hostName = argv[2];

  init_nodes(Nodes, "config.txt", nodeName);
  commandMap["Send"] = 1;
  commandMap["delete"] = 2;
  commandMap["get"] = 3;
  commandMap["insert"] = 4;
  commandMap["update"] = 5;
  commandMap["show-all"] = 6;
  commandMap["search"] = 7;
  /*
  // Checking if it built the other nodes info correctly from the config
  for (std::map<std::string,std::pair<int,int>>::iterator it=Nodes->nodeInfo.begin(); it!=Nodes->nodeInfo.end(); ++it)
    std::cout << it->first;
  */
  int portno, delay;

  portno = myconfig->nodeInfo[nodeName].first;
  delay = myconfig->nodeInfo[nodeName].second;

  // display the nodinfo
  std::cout << "\nNodeInfo:  NodeName : Port no : Delay (s) " << std::endl;
  std::cout << "\t\t" << nodeName << "\t" << portno << "\t" << delay << "\n\n";

  std::thread server_thread (server, portno);
  std::thread client_thread (client, hostName, nodeName);
  std::thread poller_thread (poller, delay, hostName);

  server_thread.join();
  client_thread.join();
  poller_thread.join();

  return 0;
}
