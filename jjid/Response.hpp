#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "uniHeader.hpp"
#include "ResponseHeader.hpp"

class Response
	{
		private:
			int statusCode;
			ResponseHeader header;
			std::string body;
	
		public:
			std::string writeResponseMessage();
			std::string searchStatusCodeMessage(int statusCode);
	};

#endif