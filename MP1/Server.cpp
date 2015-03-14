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
#include <queue>
#include <ctime>

#define SERVER 12350

using namespace std;

ConfigReader *myconfig = new ConfigReader();

std::queue<std::pair <std::string, const auto>> messageQ_A;
std::queue<std::pair <std::string, const auto>> messageQ_B;
std::queue<std::pair <std::string, const auto>> messageQ_C;
std::queue<std::pair <std::string, const auto>> messageQ_D;

std::mutex mtx1;           // mutex for critical section
std::mutex mtx2;           // mutex for critical section
std::mutex mtx3;           // mutex for critical section
std::mutex mtx4;           // mutex for critical section

void thread_A()
{
  int portA, delayA;
  std::string hostname = "localhost";

  portA = myconfig->nodeInfo["A"].first;
  delayA = myconfig->nodeInfo["A"].second;

  using std::chrono::system_clock;
  using std::chrono::duration_cast;

  ClientSocket client;

  while(1)
  {
    mtx1.lock();
    std::this_thread::sleep_for (std::chrono::seconds(1));
    const auto start = system_clock::now();

    if(!messageQ_A.empty())
    {
      std::pair <std::string, const auto> temp = messageQ_A.front();

      std::string inputMessage = temp.first;
      const auto now = temp.second;
      // Deserialize the message to get nodename and
      std::map<std::string, std::string> info = MessageHandler::deserialize(inputMessage);
      const auto delay = duration_cast<seconds>(stop - start).count();

      if(delay > delayA)
      {
        messageQ_A.pop();
        client.create();
        client.connect(portA, hostname);

        client.write(inputMessage);
        client.close();
      }

    }
    mtx1.unlock();
  }

}
/*
void thread_B()
{
  int portB, delayB;
  std::string hostname = "localhost";

  portB = myconfig->nodeInfo["B"].first;
  delayB = myconfig->nodeInfo["B"].second;

  using std::chrono::system_clock;
  using std::chrono::duration_cast;

  ClientSocket client;

  while(1)
  {
    mtx2.lock();
    std::this_thread::sleep_for (std::chrono::seconds(1));
    const auto start = system_clock::now();

    if(!messageQ_B.empty())
    {
      std::pair <std::string, const auto> temp = messageQ_B.front();

      std::string inputMessage = temp.first;
      const auto now = temp.second;
      // Deserialize the message to get nodename and
      std::map<std::string, std::string> info = MessageHandler::deserialize(inputMessage);
      const auto delay = duration_cast<seconds>(stop - start).count();

      if(delay > delayB)
      {
        messageQ_B.pop();
        client.create();
        client.connect(portB, hostname);

        client.write(inputMessage);
        client.close();
      }

    }
    mtx2.unlock();
  }

}
void thread_C()
{
  int portC, delayC;
  std::string hostname = "localhost";

  portC = myconfig->nodeInfo["C"].first;
  delayC = myconfig->nodeInfo["C"].second;

  using std::chrono::system_clock;
  using std::chrono::duration_cast;

  ClientSocket client;

  while(1)
  {
    mtx3.lock();
    std::this_thread::sleep_for (std::chrono::seconds(1));
    const auto start = system_clock::now();

    if(!messageQ_C.empty())
    {
      std::pair <std::string, const auto> temp = messageQ_C.front();

      std::string inputMessage = temp.first;
      const auto now = temp.second;
      // Deserialize the message to get nodename and
      std::map<std::string, std::string> info = MessageHandler::deserialize(inputMessage);
      const auto delay = duration_cast<seconds>(stop - start).count();

      if(delay > delayC)
      {
        messageQ_C.pop();
        client.create();
        client.connect(portC, hostname);

        client.write(inputMessage);
        client.close();
      }

    }
    mtx3.unlock();
  }

}

void thread_D()
{
  int portD, delayD;
  std::string hostname = "localhost";

  portD = myconfig->nodeInfo["D"].first;
  delayD = myconfig->nodeInfo["D"].second;

  using std::chrono::system_clock;
  using std::chrono::duration_cast;

  ClientSocket client;

  while(1)
  {
    mtx4.lock();
    std::this_thread::sleep_for (std::chrono::seconds(1));
    const auto start = system_clock::now();

    if(!messageQ_D.empty())
    {
      std::pair <std::string, const auto> temp = messageQ_D.front();

      std::string inputMessage = temp.first;
      const auto now = temp.second;
      // Deserialize the message to get nodename and
      std::map<std::string, std::string> info = MessageHandler::deserialize(inputMessage);
      const auto delay = duration_cast<seconds>(stop - start).count();

      if(delay > delayD)
      {
        messageQ_D.pop();
        client.create();
        client.connect(portD, hostname);

        client.write(inputMessage);
        client.close();
      }

    }
    mtx4.unlock();
  }

}
*/
void client(int portno, std::string hostname, std::string nodename)
{
  using std::chrono::system_clock;
  using std::chrono::duration_cast;

  ClientSocket client;

  while(1)
  {
    mtx1.lock();
    std::this_thread::sleep_for (std::chrono::seconds(1));
    const auto start = system_clock::now();

    if(!messageQ.empty())
    {
      std::pair <std::string, const auto> temp = messageQ.front();

      std::string inputMessage = temp.first;
      const auto now = temp.second;
      // Deserialize the message to get nodename and
      std::map<std::string, std::string> info = MessageHandler::deserialize(inputMessage);
      const auto delay = duration_cast<seconds>(stop - start).count();

      if(delay > myconfig->nodeInfo[info["nodeName"]].second)
      {
        int sendportno = myconfig->nodeInfo[info["nodeName"]].first;
        client.create();
        client.connect(sendportno, hostname);

        client.write(nodename);
        client.close();
      }

    }
    mtx1.unlock();
  }

}

void server()
{
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
  std::pair <std::string, const auto> temp;
  temp.first = inputCommand;
  temp.second =
  mtx1.lock();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  messageQ_A.push(temp);
  std::cout << "\nPushed to the message queue A";
  mtx1.unlock();

  mtx2.lock();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  messageQ_B.push(temp);
  std::cout << "\nPushed to the message queue B";
  mtx2.unlock();

  mtx3.lock();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  messageQ_C.push(temp);
  std::cout << "\nPushed to the message queue C";
  mtx3.unlock();

  mtx4.lock();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  messageQ_D.push(temp);
  std::cout << "\nPushed to the message queue D";
  mtx4.unlock();

  newserver.write("Message Pushed to Queues");
  newserver.close();
  }
  server.close();
}



/*
void get_input(std::string nodename)
{
  std::string inputMessage;
  while(1)
  {
    using std::chrono::system_clock;
    const auto now = system_clock::now();
    std::pair <std::string, const auto> temp;
    inputMessage = MessageHandler::serialize(inputMessage) + ":" + nodename;

    temp.first = inputMessage;
    temp.second = now;

    mtx.lock();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    messageQ.push(temp);
    std::cout << "\nPushed to the message queue";
    mtx.unlock();
  }
}
*/
int main(int argc, char *argv[])
{
  myconfig->readConfig("config.txt");

  std::thread server_thread (server);
  std::thread client_thread (client);
  //std::thread input_thread (get_input);
  std::thread poll_thread_A (thread_A);
  /*std::thread poll_thread_B (thread_B);
  std::thread poll_thread_C (thread_C);
  std::thread poll_thread_D (thread_D);
*/

  server_thread.join();
  client_thread.join();
  poll_thread_A.join();
  /*poll_thread_B.join();
  poll_thread_C.join();
  poll_thread_D.join();
  //input_thread.join();
  */
}
