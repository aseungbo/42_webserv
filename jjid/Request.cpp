#include "Request.hpp"

void RequestHeader::setHost(std::string host)
{
	this->host = host;
}

void Request::parseRequestMessage(std::string requestMessage)
{
	// msg parsing

	// startline
	startline.method = 0;
	startline.path = "/";
	startline.http = "HTTP/1.1";

	// header
	header.setHost("127.0.0.1");
	
	// body
	body = "Something JJid";
	
	// cgi
	cgi = false;
	
	// Receive Test
	std::cout << requestMessage << std::endl;
}

t_StartLine Request::getStartLine()
{
	return (startline);
}

RequestHeader& Request::getRequestHeader()
{
	return (header);
}

std::string Request::getBody()
{
	return (body);
}