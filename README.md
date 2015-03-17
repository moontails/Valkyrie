# Project Valkyrie
A distributed key-value storage system that provides
- Linearizability
- Sequential Consistency 
- Eventual Consistency 

Our storage is composed of 4 servers and each server may potentially store the value for each key in the shared datastore. The storage sytem considers both Key and Value to be positive integers.

In the first step, we implemented a communication between servers that simulates a delay. Step two was built upon step one to implement the Key-Value Store with different consistency models. A central server is used to provide totally ordered broadcast. 

# Features
- Search function that displays the list of servers that store replicas of the key being searched.
- Display function that the key-value pairs stored on the node performing this command.
- Inconsistency Repair to tackle different replicas not being consistent with each other (due to eventual consistency). We have a monitoring tool that runs periodically and ensures that all replicas eventually receive all updates.

# Compiling
The project can be compiled using the folloing commands.

1) Compiling the Node

    g++ -o node Node.cpp ConfigReader.cpp SocketBroker.cpp MessageHandler.cpp Storage.cpp -pthread -std=c++11
    
2) Compiling the Server

    g++ -o server Server.cpp ConfigReader.cpp SocketBroker.cpp MessageHandler.cpp Storage.cpp -pthread -std=c++11
