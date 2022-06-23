#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include "uniHeader.hpp"
# include "Server.hpp"
# include "Parser.hpp"

#define DEFAULT_PATH "./default.conf"

class WebServer
{
	private :
		WebServer();
		std::vector<Server> servers;
		std::string confPath;
		std::vector < int > serverFd;
		std::map < int, Server > serverMap;
		
	public : 
		WebServer(std::string confPath);
		int parseConfig();
		void makeKqueue();
		void listenServers();
		void mapFd();
		void monitorKqueue();
		bool checkLastChunked(std::string const &str);
		void disconnect_client(int client_fd, Server &currServer, std::map<int, int> &clientsServerMap);
};

#endif