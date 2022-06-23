#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "uniHeader.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "Location.hpp"

#define NOT 0
#define _DIR 1
#define _FILE 2

#define DEFAULT_INDEX "youpi.bad_extension"
#define DEFAULT_ROOT "."

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
	enum METHOD_NAME {GET, HEAD, POST, DELETE, PUT};
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
		int clientBodySize;
		
		int vecIdx;
		int writeCnt;

	public:
		int &getVecIdx();
		void setVecIdx(int idx);
		int &getWriteCnt();
		void setWriteCnt(int cnt);
		Client(int _clientSocket);
		Client(){}
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

		void setRoot(std::string root);
		void setHosts(std::vector<std::string> hosts);
		void setPort(int port);
		void setClientBodySize(int clientBodySize);
		void setIndex(std::vector<std::string> index);
		void setErrPage(std::map<std::vector<int>, std::string> errPage);
		void setLocation(std::vector<Location> locations);
		void setAllowMethod(std::vector<std::string> allowMethod);
		void setCgiPid(int pid);
		
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
		void getMethod(int isHead);
		void postMethod();
		void deleteMethod();
		
		int getServerStatus();
		void setServerStatus(int serverStatus);
		
		Location whereIsLocation(std::string const & path);
		Location getDefaultLocation();
		
		int serchIndex(std::string &path, Location currLocation, int flag);
		void openFile(std::string path, int isHead);
		
		
		void setErrorResponse(int statusCode);
		bool cgiLocation(std::string const &path);
		
		void setStatus(int stat);
		int getStatus();
		void setLocations (std::vector<Location> _locations);
		std::vector<Location> getLocations ();
		std::string &getChunkedStr();
		void addChunkedStr(std::string str);
		void setChunkedStr(std::string str);
		
		void preProcess(int type);
		int checkMethod();
		Location &getCurrLocation();
		void setCgiEvent(std::vector <struct kevent> &change_list);
		void linkFdManager(std::map<int, int> &FdManager);
		void linkChangeList(std::vector <struct kevent> &changeList);
		void setFdManager(int fd, int cleintFd);
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
		std::map< int, Client> &getClientMap();
		
};

#endif