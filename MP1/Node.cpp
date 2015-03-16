/*
 * server.cpp
 *
 *  Created on: Mar 8, 2015
 *      Author: moontails
 */

#include "headers/SocketBroker.h"
#include "headers/ConfigReader.h"
#include "headers/MessageHandler.h"
#include "headers/Storage.h"

#include <thread>
#include <ctime>
#include <ratio>
#include <chrono>
#include <fstream>
#include <map>
#include <vector>
#include <queue>
#include <mutex>
#include <sstream>

#define SERVER 12350

using namespace std;


Storage *keyvalStore = new Storage(); // local replica of the key value store

ConfigReader *myConfig = new ConfigReader(); // to store all the node information
ConfigReader *nodes = new ConfigReader();

std::map<std::string, int> commandMap; // to map command to a numeral

std::queue<std::pair <std::string, std::chrono::system_clock::time_point>> messageQ; // to hold the messages

std::mutex mtx1; // mutex lock to gurantee mutual exclusion while reading the message queue
std::mutex mtx2;
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

std::string EC_handler(std::string inputMessage)
{
  std::vector<std::string> inputMessageVector = MessageHandler::deserialize(inputMessage);
  //std::string command = inputMessageVector.front();
  int key = std::stoi(inputMessageVector[1]);
  std::pair<int, std::chrono::system_clock::time_point> temp (keyvalStore->key_value[key]);

  std::string result;

  std::ostringstream oss;
  oss << temp.first << "#" << std::chrono::system_clock::to_time_t(temp.second);
  result = oss.str();

  return result;
}
/*
* Function - Extracts the operation and operands from the input message and finally executes it on local replica.
*/
void execute_operation(std::string inputMessage)
{
  std::cout << "\nInput message is-" << inputMessage << std::endl;
  std::vector<std::string> inputMessageVector = MessageHandler::deserialize(inputMessage);
  std::string command = inputMessageVector.front();
  int key, val;
  //std::cout << "\n Command is-" << command <<std::endl;
  switch(commandMap[command])
  {
    case 1: // Send

      break;

    case 2: // delete
      key = std::stoi(inputMessageVector[1]);
      keyvalStore->deleter(key);
      break;

    case 3: // get
      key = std::stoi(inputMessageVector[1]);
      keyvalStore->getter(key);
      break;

    case 4: // insert
      key = std::stoi(inputMessageVector[1]);
      val = std::stoi(inputMessageVector[2]);
      keyvalStore->inserter(key, val);
      break;

    case 5: // update
      key = std::stoi(inputMessageVector[1]);
      val = std::stoi(inputMessageVector[2]);
      keyvalStore->updater(key, val);
      break;

    case 6: // show-all
      keyvalStore->show_all();
      break;

    case 7: // search
      search_all(inputMessage);
      break;

    default: // invalid command
      std::cout << "\nInvalid Command, please enter one of {Send, delete, get, insert, update, show-all, search}" << std::endl;
      break;
  }

}

void send_for_eventual(std::string inputMessage, std::string hostname, std::string modelNumber)
{
  std::vector<std::string> inputMessageVector = MessageHandler::deserialize(inputMessage);
  int key = std::stoi(inputMessageVector[1]);

  ClientSocket client;
  std::string message, timestamp;

  if(modelNumber == "3")
  {
    //std::cout << "\nModelNumber is " << modelNumber << std::endl;
    auto it = nodes->nodeInfo.begin();
    std::advance(it, rand() % nodes->nodeInfo.size());
    int random_portno = it->second.first;

    client.create();

    client.connect(random_portno, hostname);
    client.write(inputMessage + ":EC");
    message = client.read();
    client.close();
    //std::cout <<"\nMessage is---> "<< message;
    int pos = message.find('#');
    timestamp = message.substr(pos+1);
    //std::cout << "\nTimestamp is " << timestamp << std::endl;
    long t_int = atoi(timestamp.c_str());

    time_t t_time = t_int;

    std::chrono::system_clock::time_point temp1 = std::chrono::system_clock::from_time_t(t_time);

    std::chrono::system_clock::time_point temp2 = keyvalStore->key_value.find(key)->second.second;

    if(temp2 > temp1)
    {
      std::cout << "\nGet A " << inputMessageVector[1] << " (" << message.substr(0,pos) << "," << std::chrono::system_clock::to_time_t(temp2) << ")" << std::endl;
      std::cout << "\nGet B " << inputMessageVector[1] << " (" << message.substr(0,pos) << "," << std::chrono::system_clock::to_time_t(temp1) << ")" << std::endl;
    }
    else
    {
      std::cout << "\nGet B " << inputMessageVector[1] << "(" << message.substr(0,pos) << "," << std::chrono::system_clock::to_time_t(temp1) << ")" << std::endl;
      std::cout << "\nGet A " << inputMessageVector[1] << "(" << message.substr(0,pos) << "," << std::chrono::system_clock::to_time_t(temp2) << ")" << std::endl;
    }

  }
  else
  {
    std::cout << "\nModelNumber is " << modelNumber << std::endl;
    auto it = nodes->nodeInfo.begin();
    std::advance(it, rand() % nodes->nodeInfo.size());
    int random_portno1 = it->second.first;

    client.create();

    client.connect(random_portno1, hostname);
    client.write(inputMessage + ":EC");
    message = client.read();
    client.close();

    std::cout << message;
    int pos = message.find('#');
    timestamp = message.substr(pos+1);
    std::cout << "\nTimestamp is " << timestamp << std::endl;
    long t_int = std::stoi(timestamp);

    time_t t_time = t_int;

    std::chrono::system_clock::time_point temp1 = std::chrono::system_clock::from_time_t(t_time);

    std::chrono::system_clock::time_point temp2 = keyvalStore->key_value.find(key)->second.second;

    it = nodes->nodeInfo.begin();
    std::advance(it, rand() % nodes->nodeInfo.size());
    int random_portno2 = it->second.first;

    while(random_portno2 == random_portno1)
    {
      it = nodes->nodeInfo.begin();
      std::advance(it, rand() % nodes->nodeInfo.size());
      random_portno2 = it->second.first;
    }

    client.create();

    client.connect(random_portno2, hostname);
    client.write(inputMessage + ":EC");
    message = client.read();
    client.close();
    std::cout << message;
    int pos1 = message.find('#');
    timestamp = message.substr(pos1+1);
    std::cout << "\nTimestamp is " << timestamp << std::endl;
    t_int = std::stoi(timestamp);

    t_time = t_int;

    std::chrono::system_clock::time_point temp3 = std::chrono::system_clock::from_time_t(t_time);

    if(temp2 > temp1)
    {
      if(temp2 > temp3)
      {
        std::cout << "\nGet " << inputMessageVector[1] << "(" << message.substr(0,pos) << "," << std::chrono::system_clock::to_time_t(temp2) << ")" << std::endl;
        std::cout << "\nGet " << inputMessageVector[1] << "(" << message.substr(0,pos) << "," << std::chrono::system_clock::to_time_t(temp1) << ")" << std::endl;
        std::cout << "\nGet " << inputMessageVector[1] << "(" << message.substr(0,pos) << "," << std::chrono::system_clock::to_time_t(temp3) << ")" << std::endl;
      }
      else
      {
        std::cout << "\nGet " << inputMessageVector[1] << "(" << message.substr(0,pos) << "," << std::chrono::system_clock::to_time_t(temp3) << ")" << std::endl;
        std::cout << "\nGet " << inputMessageVector[1] << "(" << message.substr(0,pos) << "," << std::chrono::system_clock::to_time_t(temp2) << ")" << std::endl;
        std::cout << "\nGet " << inputMessageVector[1] << "(" << message.substr(0,pos) << "," << std::chrono::system_clock::to_time_t(temp1) << ")" << std::endl;
      }
    }
    else
    {
      if(temp1 > temp3)
      {
        std::cout << "\nGet " << inputMessageVector[1] << "(" << message.substr(0,pos) << "," << std::chrono::system_clock::to_time_t(temp1) << std::endl;
        std::cout << "\nGet " << inputMessageVector[1] << "(" << message.substr(0,pos) << "," << std::chrono::system_clock::to_time_t(temp2) << std::endl;
        std::cout << "\nGet " << inputMessageVector[1] << "(" << message.substr(0,pos) << "," << std::chrono::system_clock::to_time_t(temp3) << std::endl;
      }
      else
      {
        std::cout << "\nGet " << inputMessageVector[1] << "(" << message.substr(0,pos) << "," << std::chrono::system_clock::to_time_t(temp3) << std::endl;
        std::cout << "\nGet " << inputMessageVector[1] << "(" << message.substr(0,pos) << "," << std::chrono::system_clock::to_time_t(temp1) << std::endl;
        std::cout << "\nGet " << inputMessageVector[1] << "(" << message.substr(0,pos) << "," << std::chrono::system_clock::to_time_t(temp2) << std::endl;
      }
    }

  }

}

/*
 * Function - Takes the input message and the hostname and sends it to the central server.
 *          - It is used to enforce linearizability and sequential consistency through a central server.
 */
void send_to_server(std::string inputMessage, std::string hostname)
{
  // create a ClientSocket object
  ClientSocket client;

  client.create();
  // sending it to server!
  client.connect(SERVER, hostname);
  //std::cout << "\nSending message to server-" <<inputMessage<<std::endl;
  client.write(inputMessage);
  client.close();
}

/*
 * Function - Takes the input message and the hostname and sends it to all the Nodes including itself.
 */
void send_broadcast(std::string inputMessage, std::string hostname)
{
  // create a ClientSocket object
  ClientSocket client;

  // broadcast it to each node
  for (std::map<std::string,std::pair<int,int>>::iterator it=myConfig->nodeInfo.begin(); it!=myConfig->nodeInfo.end(); ++it)
  {
    client.create();
    client.connect(it->second.first, hostname);
    std::cout << "\nSending message-" <<inputMessage<<std::endl;
    client.write(inputMessage);
    client.close();
  }
}

/*
 * Function - it determines the action to take for the invoked command based on the model and operation type.
 */
void model_based_broadcast(std::string inputMessage, std::string hostname)
{
  // deserialize the message first
  std::vector<std::string> inputMessageVector = MessageHandler::deserialize(inputMessage);

  // obtain the command invoked
  std::string command = inputMessageVector.front();

  // for commands get
  if(command == "get")
  {
    std::string modelNumber = inputMessageVector.back();
    // Linearizability - send to central server
    if(modelNumber == "1")
    {
      // send to server to broadcast
      send_to_server(inputMessage, hostname);
      return;
    }
    // Sequential Consistency - return local replica
    else if(modelNumber == "2")
    {
      // read the local replica
      execute_operation(inputMessage);
      return;
    }
    else
    {
      // call eventual consistency
      std::cout << "\nEventual Stage" << std::endl;
      send_for_eventual(inputMessage, hostname, modelNumber);
    }
  }
  // for commands insert, update, send to CentralServer for model 1 and 2
  else if(command == "insert" || command == "update")
  {
    std::string modelNumber = inputMessageVector.back();
    if(modelNumber == "1" || modelNumber == "2")
    {
      send_to_server(inputMessage, hostname);
      return;
    }
    else
    {
      // call eventual consistency
      send_for_eventual(inputMessage, hostname, modelNumber);
    }
  }
  else
  {
    // else just broadcast for other cases
    send_broadcast(inputMessage, hostname);
  }

}

/*
 * Function - Poller thread to poll the message queue, and check if the delay amount of time has passed.
 *          - it then hands over the message to model_based_broadcast.
 */
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
      //std::cout << "\nFront of MQ-" << inputMessage << std::endl;
      start = temp.second;
      // Deserialize the message to get nodename and
      //std::map<std::string, std::string> info = MessageHandler::deserialize(inputMessage);
      std::chrono::seconds delay = std::chrono::duration_cast<std::chrono::seconds>(stop - start);

      if(int(delay.count()) > nodeDelay)
      {
        mtx1.lock();
        messageQ.pop();
        mtx1.unlock();

        // hand it over to model_based_broadcast to take care of sending the message to other nodes/server.
        model_based_broadcast(inputMessage, hostname);
      }

    }

  }
}

/*
 * Function - Client thread to obain user input command, affix timestamp to the command and push it into queue.
 */
void client(std::string hostName, std::string nodeName)
{
  using std::chrono::system_clock;

  std::pair <std::string, std::chrono::system_clock::time_point> temp;

  // to store time stamp variable
  std::chrono::system_clock::time_point time_point;

  // to store user input command
  std::string inputMessage;

  std::time_t timestamp;

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
    std::cout << "Serialized Message is-" << inputMessage << std::endl;
    temp.second = time_point;

    // acquire mutex lock and push into the current node's message queue and then unlock it.
    mtx1.lock();
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    messageQ.push(temp);
    std::cout << "\nPushed to the message queue at " << ctime(&timestamp);
    mtx1.unlock();
  }
}

/*
 * Function - recieves the search command, broadcasts request to all nodes, recieves responses,
 *            and creates a string based on which nodes responded positively
  */   
void search_all(std::string inputMessage)
{

  std::string response;
  std::string output;
  for(std::map<std::string, std::pair<int,int>>::iterator it = myConfig->nodeInfo.begin(); it != myConfig->nodeInfo.end(); ++it)
    {
      client.create();
      client.connect(it->second.first, hostname);
      client.write(inputMessage);
      response = client.read();
      client.close();
      if(response == "Y"){
        output.append(it->first);
        output.append(" ");
      }
    }
    std::cout<<output<<std::endl;
}

/*
 * Function - Searches local key value store for the given key. If found, returns Y, else N
  */
std::string searcher(std::string input)
{
  std::vector<std::string> inputMessageVector = MessageHandler::deserialize(input);
  std::string found;
  int temp = keyvalStore->getter(inputMessageVector[1]);
  if(temp ==-1){
    found = "N";
  }
  else
    found = "Y";
  return found;
}

/*
 * Function - Server thread to listen for incoming messages.
 *          _ It then hands it over the message to the execute_operation.
 */
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
  if( inputCommand.substr(inputCommand.length()-2) == "EC" )
  {
    std::string outputMessage = EC_handler(inputCommand);
    std::cout << "\nOutput from EC node # " << outputMessage << std::endl;
    newserver.write(outputMessage);
  }
  else if( inputCommand.substr(0,7) == "request")
  {
    std::cout << "\nProcessing repair request" << std::endl;
    std::string outputMessage = MessageHandler::serialize_map(keyvalStore->key_value);
    newserver.write("return"+outputMessage);
  }
  else if( inputComand.substr(0,6) == "return")
  {
    repair_store()
  }
  else if(inputCommand.substr(0,6) == "search")
  {
    std::string outputMessage = searcher(inputCommand);
    newserver.write(outputMessage);
  }
  else
  {
    execute_operation(inputCommand);
  }
  //newserver.write(inputCommand);
  newserver.close();
  }
  server.close();
}

/*
 * Function - Main function to -
              - initialize the node info from config file.
              - map commands/operation to a numeral.
              - display the node information.
              - spawn three threads to simulate distributed key value store.
 */
int main(int argc, char *argv[])
{
  if(argc < 3)
  {
    std::cout << "Usage: Nodename (A or B or C or D) Hostname " << std::endl;
    exit(0);
  }

  // read the information about nodes
  myConfig->readConfig("config.txt");

  std::string nodeName = argv[1];
  std::string hostName = argv[2];

  init_nodes(nodes, "config.txt", nodeName);
  commandMap["Send"] = 1;
  commandMap["delete"] = 2;
  commandMap["get"] = 3;
  commandMap["insert"] = 4;
  commandMap["update"] = 5;
  commandMap["show-all"] = 6;
  commandMap["search"] = 7;

  int portno, delay;

  // obtain the port number and delay of current node
  portno = myConfig->nodeInfo[nodeName].first;
  delay = myConfig->nodeInfo[nodeName].second;

  // display the current nodinfo
  std::cout << "\n=========================================" << std::endl;
  std::cout << "\nNodeInfo:  NodeName : Port no : Delay (s) " << std::endl;
  std::cout << "\t\t" << nodeName << "\t" << portno << "\t" << delay << std::endl;
  std::cout << "\n=========================================" << std::endl;

  // spawn the three threads
  std::thread server_thread (server, portno);
  std::thread client_thread (client, hostName, nodeName);
  std::thread poller_thread (poller, delay, hostName);

  // wait for them to complete their execution
  server_thread.join();
  client_thread.join();
  poller_thread.join();

  return 0;
}
