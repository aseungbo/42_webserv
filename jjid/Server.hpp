#ifndef SERVER_HPP
# define SERVER_HPP

# include "uniHeader.hpp"
# include "Request.hpp"
# include "Response.hpp"

// autoindex

#include "Location.hpp"
#include "Client.hpp"



// #define LOCATIONTYPE_NORMAL 0
// #define LOCATIONTYPE_REDIR 1
// #define LOCATIONTYPE_CGI 2
// #define LOCATIONTYPE_CGI_DONE 3

class Client;

class Server 
	{
		
		private:
			// Only test
			std::map< int, Client > clientMap;
			
			
			std::vector<std::string> hosts;//Host 클래스로 만들어서 호스트정보는 저장하고 메서드로 아이피 주소 퉤 하는 거 만들고싶다 
			int port;
			int clientBodySize;//이거 리퀘스트냐?? 리스폰스냐? 나중에 찌가 알아오기//ToooooooooDoooooooo*******!!!!!:클라이언트에 넘겨주기
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
			// set
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
			
			
			
			// get
			std::string getRoot();
			std::vector<std::string> getHost();
			int getPort();
			int getClientBodySize();
			std::vector<std::string> getIndex();
			std::map<int, std::string> getErrPage();
			std::vector<Location> getLocations();
			std::vector<std::string> getAllowMethod();
			// Request& getRequestClass();
			// Response& getResponseClass();
			
			void linkFdManager(std::map<int, int> &FdManager);
			void linkChangeList(std::vector <struct kevent> &changeList);
			void addClient(int clientSocket);
			
			bool findClientFd(int _fd)
			{
				for (std::map< int, Client >::iterator findIter = clientMap.begin(); findIter != clientMap.end(); findIter++)
				{
					if (findIter->second.getClientSocket() == _fd)
						return true;
				}
				return false;
			}
			// Client &getClientMap$(int _fd)
			// {
			// 	for (std::map< int, Client >::iterator findIter = clientMap.begin(); findIter != clientMap.end(); findIter++)
			// 	{
			// 		if (findIter->second.getClientSocket() == _fd)
			// 			return clientMap[_fd];
			// 	}
			// 	return ;
			// }
			
			std::map< int, Client > &getClientMap()
			{
				return clientMap;
			}

	};



#endif