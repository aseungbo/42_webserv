#include "Server.hpp"

void Server::setHost(std::vector<std::string> hosts)
{
	this->hosts = hosts;
}

void Server::setPorts(int port)
{
	this->port = port;
}

void Server::setClientBodySize(int clientBodySize)
{
	this->clientBodySize = clientBodySize;
}

// void Server::setLocation(std::vector<Location> locations);
// void Server::setErrPage(std::map<std::vector<int>, std::string> errPage);

std::vector<std::string> Server::getHost()
{
	return (hosts);
}

int Server::getPort()
{
	return (port);
}

int Server::getClientBodySize()
{
	return (clientBodySize);
}

Request& Server::getRequestClass()
{
	return (currRequest);
}

Response& Server::getResponseClass()
{
	return (currResponse);
}

void Server::processMethod()
{
	switch (currRequest.getStartLine().method)
	{
		case GET:
			getMethod();
			break;
		case HEAD:
			headMethod();
			break;
		case POST:
			postMethod();
			break;
		case DELETE:
			deleteMethod();
			break;
		default:
			//405와 같은 에러코드 처리
			break;
	}
}


void Server::getMethod()
{
	std::cout << "get "		<< std::endl;
}

void Server::postMethod()
{
		
}

void Server::deleteMethod()
{
		
}

void Server::headMethod()
{
		
}
