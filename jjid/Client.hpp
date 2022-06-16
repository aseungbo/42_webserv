#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "uniHeader.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "Location.hpp"

// # include "RequestHeader.hpp"

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

#define SERVER_READY 0
#define SERVER_ING 1

void change_events(std::vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
class Client
{
	enum METHOD_NAME {GET, HEAD, POST, DELETE};
	private:
		int clientSocket;
		std::string clientBody;
		
		std::string chunkedStr;
		int chunkedWriteSize;
		int chunkedSize;
		
		Request currRequest;
		Response currResponse;
		int status;
		Location currLocation;
		int serverFd;
		int readFd[2];
		int writeFd[2];
		pid_t cgiPid;
		char **envp;
		int serverStatus;
				
		std::map<int, int> *fdManager;
		std::vector <struct kevent> *changeList;
		std::vector<Location> locations;
		
		std::string readBuf;
		std::string FDreadBuf;
		
		int clientBodySize;
		
		
	public:
		Client(int _clientSocket);
		Client(){}
		int vecIdx;
		int writeCnt;
		std::string &getReadBuf(){return readBuf;}
		std::string &getFDReadBuf(){return FDreadBuf;}
		// void setReadBuf(char *buf){readBuf = buf;}
		
		int &getClientSocket();
		std::string &getClientBody();		
		void setClientSocket(int _fd);
		void setClientBody(std::string _body);
		
		
		char  **makeEnvp(int length);
		pid_t getCgiPid();
		void forkCgiPid();
		
		int *getReadFd();
		int *getWriteFd();
			
		void setReadFd();
		void setWriteFd();	
	
		void setStatus(int stat){status = stat;}
		int getStatus(){return (status);}
		void setRoot(std::string root);
		void setHosts(std::vector<std::string> hosts);
		void setPort(int port);
		void setClientBodySize(int clientBodySize);
		void setIndex(std::vector<std::string> index);
		void setErrPage(std::map<std::vector<int>, std::string> errPage);
		void setLocation(std::vector<Location> locations);
		void setAllowMethod(std::vector<std::string> allowMethod);
		void setCgiPid(int pid);
		
		// get
		std::string getRoot();
		std::vector<std::string> getHost();
		int getPort();
		int getClientBodySize();
		std::vector<std::string> getIndex();
		std::map<std::vector<int>, std::string> getErrPage();
		
		std::vector<std::string> getAllowMethod();
		Request& getRequestClass();
		Response& getResponseClass();
			
		
		void processMethod(std::vector <struct kevent> &change_list);
		// Request Method
		void getMethod(int isHead);
		void postMethod();
		void deleteMethod();
		// void headMethod();
		
		int getServerStatus();
		void setServerStatus(int serverStatus);
		
		// Location whereIsLocation(std::string &path, std::vector<Location> locations);
		Location whereIsLocation(std::string const & path);
		Location getDefaultLocation();
		
		int serchIndex(std::string &path, Location currLocation);
		void openFile(std::string path, int isHead);
		
		
		void setErrorResponse(int statusCode);
		bool cgiLocation(std::string const &path);
		
		
		void setLocations (std::vector<Location> _locations){locations = _locations;}
		std::vector<Location> getLocations (){return (locations);}
		std::string &getChunkedStr();
		void addChunkedStr(std::string str);
		void setChunkedStr(std::string str);
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
		void linkChangeList(std::vector <struct kevent> &changeList);
		void setFdManager(int fd, int cleintFd);
		// void Server::setCgiReadEvent(std::vector <struct kevent> &change_list);
		void setServerFd(int fd);
		int getServerFd();
		void readFile(int fd);
		void writeFile(int fd);
		
		void resetServerValues();
		
		void parseChunkedBody();
		bool checkAllowMethod(std::vector<std::string> strVec, int method);
		bool checkClientMaxSize(int locatoinClientMaxSize , int currRequestSize);
		int &getChunkedWriteSize();
		void setChunkedWriteSize(int size);
		void addChunkedWriteSize(int size);
		
		int &getChunkedSize();
		void setChunkedSize(int size);

		
		
		std::string autoIndexBody();
		void addClient(int clientSocket);
		std::map< int, Client> getClientMap();
		
};

#endif