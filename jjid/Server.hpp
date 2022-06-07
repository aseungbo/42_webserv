#ifndef SERVER_HPP
# define SERVER_HPP

# include "uniHeader.hpp"
# include "Request.hpp"
# include "Response.hpp"

// autoindex
#include <dirent.h>
#include <sys/types.h>

#include <sys/stat.h>
#include <cstdio>
#include <fcntl.h>
#include "Location.hpp"

#define NOT 0
#define _DIR 1
#define _FILE 2

// #define DEFAULT_INDEX "index.html"
#define DEFAULT_INDEX "youpi.bad_extension"
#define DEFAULT_ROOT "."
// #define LINK 3

#define YES_HEAD 1
#define NO_HEAD 0

#define ADDED_INDEX 1
#define ADD_INDEX_FAIL 0

#define READY 0
#define DONE 1
#define CHUNKED 2
#define CHUNKED_ALIVE 3
#define CHUNKED_DONE 4
#define CHUNKED_FIN 5

// #define LOCATIONTYPE_NORMAL 0
// #define LOCATIONTYPE_REDIR 1
// #define LOCATIONTYPE_CGI 2
// #define LOCATIONTYPE_CGI_DONE 3

void change_events(std::vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
class Server 
	{
		enum METHOD_NAME {GET, HEAD, POST, DELETE};
		private:
			
			std::vector<std::string> hosts;//Host 클래스로 만들어서 호스트정보는 저장하고 메서드로 아이피 주소 퉤 하는 거 만들고싶다 
			int port;
			int clientBodySize;//이거 리퀘스트냐?? 리스폰스냐? 나중에 찌가 알아오기
			std::string root;
			std::vector<std::string> index;
			std::vector<Location> locations;
			std::map<std::vector<int>, std::string> errPage;
			std::vector<std::string> allowMethod;
			
			//서버 상태 필요할듯 청크에서 등
			Request currRequest;
			Response currResponse;//구조적으로 맘에 안듦 (jji, 29, 무직)
			
			// int cgiWriteFd[2];
			// int cgiReadFd[2];
			// std::string cgiBody;
			
			// test
			int clientSocket;
			int status;
			int chunkedSize;
			int currChunkedSize;
			Location currLocation;
			
			std::map<int, int> *fdManager;
			int serverFd;
			bool fdFlag;//resource or cgi
			std::vector <struct kevent> *changeList;
			
			int readFd[2];
			int writeFd[2];
			pid_t cgiPid;
			
			char **envp;
			
		public:

		char  **makeEnvp();
		pid_t getCgiPid();
		void forkCgiPid();
		
		int *getReadFd();
		int *getWriteFd();
			
		void setReadFd();
		void setWriteFd();	
	
		void setStatus(int stat){status = stat;}
		int getStatus(){return (status);}
		void setChunkedSize(int size){chunkedSize = size;}
		int getChunkedSize(){return (chunkedSize);}
		void setCurrChunkedSize(int size){currChunkedSize = size;}
		int getCurrChunkedSize(){return (currChunkedSize);}
			// Server();
			
			// test
			int getClientSocket() { return clientSocket; }
			void setClientSocket(int socketFd) { this->clientSocket = socketFd; }
			
			// set
			void setRoot(std::string root);
			void setHosts(std::vector<std::string> hosts);
			void setPort(int port);
			void setClientBodySize(int clientBodySize);
			void setIndex(std::vector<std::string> index);
			void setErrPage(std::map<std::vector<int>, std::string> errPage);
			void setLocation(std::vector<Location> locations);
			void setAllowMethod(std::vector<std::string> allowMethod);
			
			// get
			std::string getRoot();
			std::vector<std::string> getHost();
			int getPort();
			int getClientBodySize();
			std::vector<std::string> getIndex();
			std::map<std::vector<int>, std::string> getErrPage();
			std::vector<Location> getLocations();
			std::vector<std::string> getAllowMethod();
			Request& getRequestClass();
			Response& getResponseClass();
			
			void processMethod(std::vector <struct kevent> &change_list);
			// Request Method
			void getMethod(int isHead);
			void postMethod();
			void deleteMethod();
			// void headMethod();
			
			
			// Location whereIsLocation(std::string &path, std::vector<Location> locations);
			Location whereIsLocation(std::string const & path);
			Location getDefaultLocation();
			
			int serchIndex(std::string &path, Location currLocation);
			void openFile(std::string path, int isHead);
			
			
			void setErrorResponse(int statusCode);
			bool cgiLocation(std::string const &path);
			
			// int *getCgiWriteFd();
			// int *getCgiReadFd();
			// std::string getCgiBody();
			// char **getEnvp();
			
			// void setCgiWriteFd(int cgiWriteFd[2]);
			// void setCgiReadFd(int cgiReadFd[2]);
			// void setCgiBody(std::string str);
			// void setEnvp(char **envp);
			
			
			
			void preProcess(int type);
			int checkMethod();
			Location &getCurrLocation();
			void setCgiEvent(std::vector <struct kevent> &change_list);
			void linkFdManager(std::map<int, int> &FdManager);
			void setFdManager(int fd, int serverFd);
			// void Server::setCgiReadEvent(std::vector <struct kevent> &change_list);
			void setServerFd(int fd);
			int getServerFd();
			void linkChangeList(std::vector <struct kevent> &changeList);
			void readFile(int fd);
			void writeFile(int fd);
			
			void resetServerValues();
			
			void parseChunkedBody();

			std::string autoIndexBody();
	};



#endif