/*
 * SocketBrokerBroker.cpp
 *
 *  Created on: Feb 21, 2015
 *      Author: moontails
 *
 *      Implementation of the SocketBroker, ClientSocket, ServerSocket classes method implementations
 */

#include "headers/SocketBroker.h"
#include "headers/MessageHandler.h"

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

std::string SocketBroker::time_printer()
{
	time_t t = time(0);   // get time now
	return asctime(localtime(&t));
}

/*
* ServerSocket class method implementations
*/

void ServerSocket::create()
{
	if( ( sb_sockfd = ::socket( AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cout << "\nCould not create Socket" << std::endl;
		exit(0);
	}
	else
	{
		std::cout << "\nNew ServerSocket created" << std::endl;
	}

}

void ServerSocket::bind(int port)
{
	// Check if a socket was created successfully
	if ( ! is_valid() )
	{
		std::cout << "\nCreate socket before bind" << std::endl;
		exit(0);
	}

	sb_sockaddr.sin_family = AF_INET;
	sb_sockaddr.sin_addr.s_addr = INADDR_ANY;
	sb_sockaddr.sin_port = htons(port);

	int bind_return = ::bind( sb_sockfd, (struct sockaddr *) &sb_sockaddr, sizeof( sb_sockaddr ) );


	if ( bind_return < 0 )
	{
		//perror("Bind Failed");
		std::cout << "\nBind Failed" << std::endl;
		exit(0);
	}
	std::cout << "\nBinded to port: " << port << std::endl;
}


void ServerSocket::listen()
{

	if ( ! is_valid() )
	{
		std::cout << "\nCreate socket before listen" << std::endl;
		exit(0);
	}
	std::cout << "\nListening:" << std::endl;
	int listen_return = ::listen( sb_sockfd, MAXCONNECTIONS );


	if ( listen_return < 0 )
	{
		std::cout << "\nListen Failed" << std::endl;
		exit(0);
	}

}


void ServerSocket::accept(ServerSocket& new_SocketBroker)
{
	std::cout << "\nAccepting:" << std::endl;
	socklen_t alen; /* length of address structure */
	new_SocketBroker.sb_sockfd = ::accept(sb_sockfd, (struct sockaddr *) &sb_sockaddr, &alen );

	if ( new_SocketBroker.sb_sockfd <= 0 )
	{
		std::cout << "\nAccept Failed" << std::endl;
		exit(0);
	}

}

std::string ServerSocket::read()
{
	char buffer[256];
	int n;

	bzero(buffer,256);

	n = ::read(sb_sockfd,buffer,256);

	/*for(int i = 0; i<256;i++)
		std::cout << buffer[i];
	printf("HERE-%d",n);
	*/
	if (n < 0)
	{
		std::cout << "\nReading at server socket Failed" << std::endl;
		exit(0);
	}

	std::string inputMessage(buffer);

	std::cout << "\nReceived message :" << inputMessage << " system time is " << SocketBroker::time_printer() << std::endl;
	return inputMessage;
}

void ServerSocket::write(std::string outputMessage)
{
	std::string result;

	int pos1 = outputMessage.find(':');
	int pos2 = outputMessage.find_last_of(':');
	std::string nodeName = outputMessage.substr(pos2-1,1);
	//result = "Sent " + outputMessage.substr(pos1+1, pos2-pos1-3) + " to " + nodeName + " " + SocketBroker::time_printer();
	result = "ACK";
	int n;

	n = ::write(sb_sockfd,result.c_str(),result.length());

	if (n < 0)
	{
		std::cout << "\nWriting at server socket Failed" << std::endl;
		exit(0);
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
		std::cout << "\nCould not create Socket" << std::endl;
		exit(0);
	}
	else
	{
		std::cout << "\nNew ClientSocket created" << std::endl;
	}

}

void ClientSocket::connect(int port, std::string hostname)
{
	if ( ! is_valid() )
	{
		std::cout << "\nSocket Error in connect" << std::endl;
		exit(0);
	}

	struct hostent *server;

	sb_sockaddr.sin_family = AF_INET;
	sb_sockaddr.sin_port = htons(port);

	server = gethostbyname(hostname.c_str());
	if(server == NULL)
	{
		std::cout << "\nERROR, no such host" << std::endl;
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
		std::cout << "\nConnect Failed" << std::endl;
		exit(0);
	}

}

std::string ClientSocket::read()
{
	char buffer[256];
	int n;

	bzero(buffer,256);

	n = ::read(sb_sockfd,buffer,MAXRECV);

	if (n < 0)
	{
		std::cout << "\nReading at client socket Failed" << std::endl;
		exit(0);
	}

	std::string unpack(buffer);
	std::cout << buffer;
	return unpack;
}

void ClientSocket::write(std::string inputMessage)
{
	//std::string inputMessage;
	int n;
	//std::cout << "\nInput -"<< inputMessage.c_str() << inputMessage.length() ;
	n = ::write(sb_sockfd,inputMessage.c_str(),inputMessage.length());
	//printf("Senteeer - %d",n);
	if (n < 0)
	{
		std::cout << "\nWriting at client socket Failed" << std::endl;
		exit(0);
	}
	std::cout << "\nMessage Sent to server" << std::endl;
}
