#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include "uniHeader.hpp"
# include "Server.hpp"
# include "parseConf.hpp"

#define DEFAULT_PATH "./default.conf"

class WebServer
	{
		private :
			WebServer();//hyopark : 안쓸 기본생성자
			std::vector<Server> servers;
			std::string confPath;
			
			std::vector < int > serverFd; //자료형 고려
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
#endif