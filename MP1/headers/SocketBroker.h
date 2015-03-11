/*
 * SocketBroker.h
 *
 *  Created on: Feb 21, 2015
 *      Author: moontails
 */

#ifndef SOCKETBROKER_H_
#define SOCKETBROKER_H_

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

const int MAXHOSTNAME = 200;
const int MAXCONNECTIONS = 5;
const int MAXRECV = 500;

using namespace std;

class SocketBroker
{
public:
	SocketBroker();
	virtual ~SocketBroker();

	// Function to create a socket
	virtual void create() = 0;

	// Function to close a socket
	void close();

	// Function to read from a socket
	virtual void read() = 0;

	// Function to write from a socket
	virtual void write() = 0;

	// Function to check if created socket is valid or not
	bool is_valid() const;

protected:
	int sb_sockfd;
	struct sockaddr_in sb_sockaddr;
};

class ClientSocket : public SocketBroker
{
public:
	void create();
	void connect(int port, std::string host);
	void read();
	void write();
};

class ServerSocket : public SocketBroker
{
public:
	void create();
	void bind(int port);
	void listen();
	void accept(ServerSocket& );
	void read();
	void write();
	//void send ( const std::string );
	//int recv ( std::string& );
};

#endif /* SOCKETBROKER_H_ */
