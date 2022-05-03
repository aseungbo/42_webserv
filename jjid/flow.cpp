	#include <vector>
	#include <iostream>
	#include <map>
	// class EventHandler
	// {
	// 	private:
	// 	// 	Request request;//
	// 	// 	Server *server;//처리될 서버
	// 	// 	int fd;//연결 유지 등 필요한 수 잇음
	// 	// 	int status;// 요청 가능 / 대기 등 
	// 	public:
	// 	// 	re
		
			
	// }
	//당신의 클라이언트 이벤트헨들러로 대체되었읍니다.
	
	
	class Location : public Server
	{
		private:
			std::string root;
			std::string index;
			// bool autoIndex;
			// std::map<std::vector<int>, std::string> errPage;
			std::vector<std::string> methodsAllowed
		public:
			void
	
	};
	
	
	class Server 
	{
		private:
			std::vector<std::string> hosts;
			std::vector<int> ports;
			std::vector<Location> locations;
			int clientBodySize;//이거 리퀘스트냐?? 리스폰스냐? 나중에 찌가 알아오기
			std::vector<std::string> allowMethod;
			//서버 상태 필요할듯 청크에서 등
			Request currRequest;
			Response currResponse;//구조적으로 맘에 안듦 (jji, 29, 무직)
			
		protected:
			std::map<std::vector<int>, std::string> errPage; // errCode , Page;
			
		public:
			Server();
			void setHost(std::vector<std::string> hosts);
			void setPorts(std::vector<int> ports);
			void setLocation(std::vector<Location> locations);
			void setClientBodySize(int clientBodySize);
			void setErrPage(std::map<std::vector<int>, std::string> errPage);
			
			// void Method
			void getMethod();
			void postMethod();
			void deleteMethod();
			void headMethod();
			
			// Response returnResponse();
	};
	
	class WebServer
	{
		private :
			std::vector<Server> servers;
			// std::vector < int > serverFd; //자료형 고려
			std::string confPath;
			std::map < int, Server > serverMap; // 서버에서 뭘 받아온다면 다시 고려 
			// EventHandler // 언젠가 꼭 고려해보쟈 ><
			
		public : 
			WebServer(std::string confPath); // ConfigParser 불러옴
			void parseConfig(); // this->servers.push_back()
			void makeKqueue(); // kqueue 생성 
			void listenServers();//서버들 리슨 바인드 까지
			void mapFd(); // servers 시작 -> serverFd; -> serverMap
			void monitorKqueue(); //->안의 기능은 따로 뺄예정
			
	};
	
	class RequestHeader : public Header
	{
		private:
			std::string host;
			// host 서ㅓㅂ의 도메인 네임
			// User-Agent 어떤 클라이언트를 통해 요청
			// Authotization 인증
						// Cookie
						// Origin
			// IF-Modified-Since (조건부 요펑이 들어오면 처리해야 할 것)
		public:
		
	};
	
	class ResponseHeader : public Header
	{
		private:
			std::string server;
			std::vector<std::string> allow;
			std::string location;
		public:
		
	};
	
	// 상속 받아서 쓸 것.
	class Header
	{
		protected:
			std::string date;
			std::string connection;
			int contentLength;
		public:
			
	};
	
	typedef struct s_StartLine
	{
		std::string method;
		std::string http;
		std::string path;
	} t_StartLine;
	
	class Request
	{
		private:
			t_StartLine startline;
			RequestHeader header;                                
			std::string body;
			bool cgi;//해당 요청(확장자 보고 판단 )이 cgi인지
			
		public:
			void parseRequestMessage();
	};
	
	class Response
	{
		private:
			int statusCode;
			RequestHeader header;
			std::string body;
	
		public:
			void writeResponseMessage();
			void searchStatusCodeMessage();
	};


int main (int ac, char **av)
{
	std::string confPath;
	if (ac == 2)
	{
		confPath = av[1];
	}
	else if (ac == 1)
	{
		confPath = DEFAULT_PATH;
	}
	else
	{
		printErr("Too many argu");
	}
	WebServer myFirstWebServer(confPath);
	myFirstWebServer.listenServers();
	myFirstWebServer.parseConfig();
	// string confPath = string checkArgu(ac, av);// -> 1 : conf파일 제대로 들어온경우 2 : default로 가야하는경우 0 : 종료해야할 경우(잘못된 파일,)
	// // WebServer a;
	// Server a;
	
	
}