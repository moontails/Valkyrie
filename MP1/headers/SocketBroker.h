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

	bool create();

	// Helper functions to instantiate server
	virtual bool bind ( const int port ) = 0;
	virtual bool listen() const =0;
	virtual bool accept ( SocketBroker& ) const = 0;
	virtual bool send ( const std::string ) const = 0;
	virtual int recv ( std::string& ) const = 0;

	// Helper functions to instantiate client
	virtual bool connect ( const std::string host, const int port ) = 0;
	virtual int read() = 0;
	virtual int write() = 0;

	// Helper function to check if created socket is valid or not
	bool is_valid() const;

protected:
	int sb_sockfd;
	struct sockaddr_in sb_sockaddr;
};

class ClientSocket : public SocketBroker
{
	// Helper functions to instantiate client
	bool connect ( const std::string host, const int port );
	int read();
	int write();
};

class ServerSocket : public SocketBroker
{
	// Helper functions to instantiate server
	bool bind ( const int port );
	bool listen() const;
	bool accept ( SocketBroker& ) const;
	bool send ( const std::string ) const;
	int recv ( std::string& ) const;
};

#endif /* SOCKETBROKER_H_ */
