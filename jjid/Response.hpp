#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "uniHeader.hpp"
#include "Header.hpp"

class Response
	{
		private:
			int statusCode;
			Header header;
			std::string body;
	
		public:
			std::string writeResponseMessage();//안에 바디사이즈로 청크드 만드는 메서드 호출 하고싶음
			std::string searchStatusCodeMessage(int statusCode);
			
			//TODO 구현
			void setStatusCode(int statusCode);
			void setHeader(Header header);
			void setBody(std::string body);
			
			int getStatusCode();
			Header getHeader();
			std::string getBody();
	};

#endif