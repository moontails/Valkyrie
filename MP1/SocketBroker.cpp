/*
 * SocketBrokerBroker.cpp
 *
 *  Created on: Feb 21, 2015
 *      Author: moontails
 *
 *      Implementation of the SocketBroker, ClientSocket, ServerSocket classes method implementations
 */

#include "headers/SocketBroker.h"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>

using namespace std;

/*
 * SocketBroker class method implementations
 */

SocketBroker::SocketBroker() : sb_sockfd ( -1 )
{
	memset( &sb_sockaddr, 0, sizeof(sb_sockaddr) );
}

SocketBroker::~SocketBroker()
{
	if(is_valid())
	{
		::close(sb_sockfd);
	}
}

bool SocketBroker::is_valid() const
{
	return sb_sockfd != -1;
}

void SocketBroker::close()
{
	::close(sb_sockfd);
}

/*
* ServerSocket class method implementations
*/

void ServerSocket::create()
{
	if( ( sb_sockfd = ::socket( AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cout << "Could not create Socket" << std::endl;
		exit(0);
	}
	else
	{
		std::cout << "New ServerSocket created" << std::endl;
	}

}

void ServerSocket::bind(int port)
{
	// Check if a socket was created successfully
	if ( ! is_valid() )
	{
		std::cout << "Create socket before bind" << std::endl;
		exit(0);
	}

	sb_sockaddr.sin_family = AF_INET;
	sb_sockaddr.sin_addr.s_addr = INADDR_ANY;
	sb_sockaddr.sin_port = htons(port);

	int bind_return = ::bind( sb_sockfd, (struct sockaddr *) &sb_sockaddr, sizeof( sb_sockaddr ) );


	if ( bind_return < 0 )
	{
		//perror("Bind Failed");
		std::cout << "Bind Failed" << std::endl;
		exit(0);
	}

}


void ServerSocket::listen()
{
	if ( ! is_valid() )
	{
		std::cout << "Create socket before listen" << std::endl;
		exit(0);
	}

	int listen_return = ::listen( sb_sockfd, MAXCONNECTIONS );


	if ( listen_return < 0 )
	{
		std::cout << "Listen Failed" << std::endl;
		exit(0);
	}

}


void ServerSocket::accept(ServerSocket& new_SocketBroker)
{
	socklen_t alen; /* length of address structure */
	new_SocketBroker.sb_sockfd = ::accept(sb_sockfd, (struct sockaddr *) &sb_sockaddr, &alen );

	if ( new_SocketBroker.sb_sockfd <= 0 )
	{
		std::cout << "Accept Failed" << std::endl;
		exit(0);
	}

}

void ServerSocket::read()
{
	char buffer[256];
	int n;

	bzero(buffer,256);

	n = ::read(sb_sockfd,buffer,MAXRECV);

	if (n < 0)
	{
		std::cout << "Reading at server socket Failed" << std::endl;
	}
	std::cout << "Here is the message: " << buffer << std::endl;

}

void ServerSocket::write()
{
	int n;

	n = ::write(sb_sockfd,"I got your message",18);

	if (n < 0)
	{
		std::cout << "Writing at server socket Failed" << std::endl;
	}

}


/*
void SocketBroker::send(const std::string s) const
{
	int status = ::send(sb_sockfd, s.c_str(), s.size(), MSG_NOSIGNAL );

	if ( status == -1 )
	{
		std::cout << "Send Failed" << std::endl;
		return false;
	}
	else
	{
		return true;
	}
}


int SocketBroker::recv(std::string& s) const
{
	char buf [MAXRECV + 1];

	s = "";

	memset ( buf, 0, MAXRECV + 1 );

	int status = ::recv(sb_sockfd, buf, MAXRECV, 0 );

	if ( status == -1 )
	{
		std::cout << "status == -1   errno == " << errno << "  in SocketBroker::recv\n";
		return 0;
	}
	else if ( status == 0 )
	{
		return 0;
	}
	else
	{
		s = buf;
		return status;
	}
}
*/

/*
 * ClientSocket class method implementations
 */

void ClientSocket::create()
{
	if( ( sb_sockfd = ::socket( AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cout << "Could not create Socket" << std::endl;
		exit(0);
	}
	else
	{
		std::cout << "New ClientSocket created" << std::endl;
	}

}

void ClientSocket::connect(int port, std::string hostname)
{
	if ( ! is_valid() )
	{
		std::cout << "Socket Error in connect" << std::endl;
		exit(0);
	}

	struct hostent *server;

	sb_sockaddr.sin_family = AF_INET;
	sb_sockaddr.sin_port = htons(port);

	server = gethostbyname(hostname.c_str());
	if(server == NULL)
	{
		std::cout << "ERROR, no such host" << std::endl;
		exit(0);
	}

	bzero((char *) &sb_sockaddr, sizeof(sb_sockaddr));
	sb_sockaddr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,	(char *)&sb_sockaddr.sin_addr.s_addr,	server->h_length);
	sb_sockaddr.sin_port = htons(port);

	//memcpy(&sb_sockaddr.sin_addr, host->h_addr_list[0], host->h_length);
	//int status = inet_pton(AF_INET, host.c_str(), &sb_sockaddr.sin_addr );

	//if ( errno == EAFNOSUPPORT ) return false;

	if(::connect(sb_sockfd, (sockaddr *)&sb_sockaddr, sizeof(sb_sockaddr) ) < 0 )
	{
		std::cout << "Connect Failed" << std::endl;
		exit(0);
	}

}

void ClientSocket::read()
{
	char buffer[256];
	int n;

	bzero(buffer,256);

	n = ::read(sb_sockfd,buffer,MAXRECV);

	if (n < 0)
	{
		std::cout << "Reading at client socket Failed" << std::endl;
	}

}

void ClientSocket::write()
{
	char buffer[256];
	int n;

	printf("Please enter the message: ");
	bzero(buffer,256);
	fgets(buffer,255,stdin);

	n = ::write(sb_sockfd,buffer,strlen(buffer));

	if (n < 0)
	{
		std::cout << "Writing at client socket Failed" << std::endl;
	}

}
