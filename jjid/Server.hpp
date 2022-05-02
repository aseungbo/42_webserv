#ifndef SERVER_HPP
# define SERVER_HPP

# include "uniHeader.hpp"

// class Location;

class Server 
	{
		private:
			std::vector<std::string> hosts;//Host 클래스로 만들어서 호스트정보는 저장하고 메서드로 아이피 주소 퉤 하는 거 만들고싶다 
			// std::vector<int> ports;//파싱할때 알아서 서버별로 포트 구분하세요~
			int port;
			// std::vector<Location> locations;
			int clientBodySize;//이거 리퀘스트냐?? 리스폰스냐? 나중에 찌가 알아오기
			std::vector<std::string> allowMethod;
			//서버 상태 필요할듯 청크에서 등
			// Request currRequest;
			// Response currResponse;//구조적으로 맘에 안듦 (jji, 29, 무직)
			
		protected:
			std::map<std::vector<int>, std::string> errPage; // errCode , Page;
			
		public:
			// Server();
			void setHost(std::vector<std::string> hosts)
			{
				this->hosts = hosts;
			}
			void setPorts(int port)
			{
				this->port = port;
			}
			// void setLocation(std::vector<Location> locations);
			void setClientBodySize(int clientBodySize)
			{
				this->clientBodySize = clientBodySize;
			}
			void setErrPage(std::map<std::vector<int>, std::string> errPage);
			
			std::vector<std::string> getHost()
			{
				return (hosts);
			}
			int getPort()
			{
				return (port);
			}
			int getClientBodySize()
			{
				return (clientBodySize);
			}
		
			
			// void Method
			void getMethod();
			void postMethod();
			void deleteMethod();
			void headMethod();
			
			// Response returnResponse();
	};



#endif