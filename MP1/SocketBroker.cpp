/*
 * SocketBrokerBroker.cpp
 *
 *  Created on: Feb 21, 2015
 *      Author: moontails
 *
 *      Implementation of the SocketBrokerBroker class.
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

//SocketBroker class method implementations

SocketBroker::SocketBroker() : sb_sockfd ( -1 )
{
	memset( &sb_sockaddr, 0, sizeof(sb_sockaddr) );
}

SocketBroker::~SocketBroker()
{
	if ( is_valid() ) ::close( sb_sockfd );
}

bool SocketBroker::is_valid() const
{
	return sb_sockfd == -1;
}

bool SocketBroker::create()
{
	if( ( sb_sockfd = ::socket( AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cout << "Could not create Socket" << std::endl;
		return false;
	}
	else
	{
		std::cout << "New Socket created" << std::endl;
		return true;
	}

}

//ServerSocket class method implementations

bool ServerSocket::bind ( const int port )
{
	// Check if a socket was created successfully
	if ( ! is_valid() )
	{
		std::cout << "Create socket before bind" << std::endl;
		return false;
	}

	sb_sockaddr.sin_family = AF_INET;
	sb_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sb_sockaddr.sin_port = htons(port);

	int bind_return = ::bind( sb_sockfd, (struct sockaddr *) &sb_sockaddr, sizeof( sb_sockaddr ) );


	if ( bind_return < 0 )
	{
		//perror("Bind Failed");
		std::cout << "Bind Failed" << std::endl;
		return false;
	}

	return true;
}


bool ServerSocket::listen() const
{
	if ( ! is_valid() )
	{
		std::cout << "Create socket before listen" << std::endl;
		return false;
	}

	int listen_return = ::listen( sb_sockfd, MAXCONNECTIONS );


	if ( listen_return < 0 )
	{
		std::cout << "Listen Failed" << std::endl;
		return false;
	}

	return true;
}


bool ServerSocket::accept ( SocketBroker& new_SocketBroker ) const
{
	socklen_t alen; /* length of address structure */
	new_SocketBroker.sb_sockfd = ::accept( sb_sockfd, (struct sockaddr *) &sb_sockaddr, &alen );

	if ( new_SocketBroker.sb_sockfd <= 0 )
	{
		std::cout << "Accept Failed" << std::endl;
		return false;
	}
	else
		return true;
}


bool ServerSocket::send ( const std::string s ) const
{
	int status = ::send( sb_sockfd, s.c_str(), s.size(), MSG_NOSIGNAL );

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


int ServerSocket::recv ( std::string& s ) const
{
	char buf [ MAXRECV + 1 ];

	s = "";

	memset ( buf, 0, MAXRECV + 1 );

	int status = ::recv ( sb_sockfd, buf, MAXRECV, 0 );

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

//ClientSocket class method implementations

bool ClientSocket::connect ( const std::string host, const int port )
{
	if ( ! is_valid() ) return false;

	sb_sockaddr.sin_family = AF_INET;
	sb_sockaddr.sin_port = htons ( port );

	int status = inet_pton ( AF_INET, host.c_str(), &sb_sockaddr.sin_addr );

	if ( errno == EAFNOSUPPORT ) return false;

	status = ::connect ( sb_sockfd, ( sockaddr * ) &sb_sockaddr, sizeof ( sb_sockaddr ) );

	if ( status == 0 )
		return true;
	else
		return false;
}





