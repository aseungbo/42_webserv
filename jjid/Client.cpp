#include "Client.hpp"

Client::Client(int _clientSocket)
{
	clientSocket = _clientSocket;
	
	clientBody.clear();
}

// int &getClientSocket();
// std::string &getClientBody();		
// void setClientSocket(int _fd);
// void setClientBody(std::string _body);