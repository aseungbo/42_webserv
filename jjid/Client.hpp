#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "uniHeader.hpp"
// # include "RequestHeader.hpp"
	
class Client
{
	
	private:
		int clientSocket;
		std::string clientBody;
		
	public:
		Client(int _clientSocket);
		
		int &getClientSocket();
		std::string &getClientBody();		
		void setClientSocket(int _fd);
		void setClientBody(std::string _body);
		
};

#endif