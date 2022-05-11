#include "Response.hpp"
	
std::string Response::writeResponseMessage()
{
	std::string returnString = "";
	// this->statusCode = 200; // test
	returnString = "HTTP/1.1 " + std::to_string(statusCode);// + searchStatusCodeMessage(statusCode) + "\n" + getBody();
	returnString = returnString + " Gooood\r\nDate: a\r\nServer: a\r\nLast-Modified: a\r\nETag: 'A'\r\nAccept-Ranges: bytes\r\nConnection: close\r\nContent-Type: text/html;charset=UTF-8\r\nContent-Length: " + std::to_string(getBody().size()) +  "\r\n\n" + getBody() ; 
	// std::cout << returnString << std::endl;
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

void Response::setHeader(Header header)
{
	this->header = header;
}

void Response::setBody(std::string body)
{
	this->body = body;
}

int Response::getStatusCode()
{
	return (statusCode);
}

Header Response::getHeader()
{
	return (header);
}

std::string Response::getBody()
{
	return (body);
}

