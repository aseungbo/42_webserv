#include "Response.hpp"
	
std::string Response::writeResponseMessage()
{
	std::string returnString = "";
	this->statusCode = 200; // test
	returnString = "HTTP/1.1 " + std::to_string(statusCode) + searchStatusCodeMessage(statusCode);
	return (returnString);
}

std::string Response::searchStatusCodeMessage(int statusCode)
{
	std::string returnStatusCodeMessage;
	
	returnStatusCodeMessage = " OK\n\n";
	return (returnStatusCodeMessage);
}

void Response::setStatusCode(int statusCode)
{
	this->statusCode = statusCode;
}

void Response::setHeader(ResponseHeader header)
{
	this->header = header;
}

void Response::setBody(std::string body)
{
	this->body = body;
}
