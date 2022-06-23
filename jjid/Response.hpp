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
		int errStatusCode;

	public:
		Response();
		std::string CgiHeader;
		std::vector <std::string> chunkedVec;
		std::string searchStatusCodeMessage(int statusCode);
		
		void setStatusCode(int statusCode);
		void setHeader(Header header);
		void setBody(std::string body);
		void setErrStatusCode(int errStatuscode);
		
		int getErrStatusCode();
		int getStatusCode();
		Header getHeader();
		std::string &getBody();
		
		std::string statusMessage(size_t code);
		std::string setErrorResponse(int statusCode);
		std::string writeResponseMessage();
};

#endif