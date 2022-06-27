#ifndef SERVER_HPP
# define SERVER_HPP

# include "uniHeader.hpp"
# include "Request.hpp"
# include "Response.hpp"

#include "Location.hpp"
#include "Client.hpp"


class Client;

class Server 
{
	private:
		std::map< int, Client > clientMap;
		std::vector<std::string> hosts;
		int port;
		int clientBodySize;
		std::string root;
		std::vector<std::string> index;
		std::vector<Location> locations;
		std::map<int, std::string> errPage;
		std::vector<std::string> allowMethod;
		int serverFd;
		std::vector <struct kevent> *changeList;
		std::map<int, int> *fdManager;
		
	public:
		Server(void);
		void setRoot(std::string root);
		void setHosts(std::vector<std::string> hosts);
		void setPort(int port);
		void setClientBodySize(int clientBodySize);
		void setIndex(std::vector<std::string> index);
		void setErrPage(std::map<int, std::string> errPage);
		void setLocation(std::vector<Location> locations);
		void setAllowMethod(std::vector<std::string> allowMethod);
		void setCgiPid(int pid);
		void setServerFd(int fd);
		std::string getRoot();
		std::vector<std::string> getHost();
		int getPort();
		int getClientBodySize();
		std::vector<std::string> getIndex();
		std::map<int, std::string> getErrPage();
		std::vector<Location> getLocations();
		std::vector<std::string> getAllowMethod();
		int getServerFd();
		void linkFdManager(std::map<int, int> &FdManager);
		void linkChangeList(std::vector <struct kevent> &changeList);
		void addClient(int clientSocket);
		std::map< int, Client > &getClientMap();
};

#endif