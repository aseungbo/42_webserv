#include "Response.hpp"
	
Response::Response()
{
	statusCode = 0;
	errStatusCode = 0;
}
	
std::string	Response::statusMessage(size_t statusCode)
{
	static std::map<size_t, std::string>	status;


	if (status.size() == 0)
	{
		status[100] = "Continue";
		status[101] = "Switching Protocols";
		status[102] = "Processing";
		status[200] = "OK";
		status[201] = "Created";
		status[202] = "Accepted";
		status[203] = "Non-authoritative Information";
		status[204] = "No Content";
		status[205] = "Reset Content";
		status[206] = "Partial Content";
		status[207] = "Multi-Status";
		status[208] = "Already Reported";
		status[226] = "IM Used";
		status[300] = "Multiple Choices";
		status[301] = "Moved Permanently";
		status[302] = "Found";
		status[303] = "See Other";
		status[304] = "Not Modified";
		status[305] = "Use Proxy";
		status[307] = "Temporary Redirect";
		status[308] = "Permanent Redirect";
		status[400] = "Bad Request";
		status[401] = "Unauthorized";
		status[402] = "Payment Required";
		status[403] = "Forbidden";
		status[404] = "Not found";
		status[405] = "Method Not Allowed";
		status[406] = "Not Acceptable";
		status[407] = "Proxy Authentication Required";
		status[408] = "Required Timeout";
		status[409] = "Conflict";
		status[410] = "Gone";
		status[411] = "Length Required";
		status[412] = "Precondition Failed";
		status[413] = "Request Entity Too Large";
		status[414] = "Request URI Too Long";
		status[415] = "Unsupported Media Type";
		status[416] = "Requested Range Not Satisfiable";
		status[417] = "Expectation Failed";
		status[418] = "IM_A_TEAPOT";
		status[500] = "Internal Server Error";
		status[501] = "Not Implemented";
		status[502] = "Bad Gateway";
		status[503] = "Service Unavailable";
		status[504] = "Gateway Timeout";
		status[505] = "HTTP Version Not Supported";
		status[506] = "Variant Also Negotiates";
		status[507] = "Insufficient Storage";
		status[508] = "Loop Detected";
		status[510] = "Not Extened";
		status[511] = "Network Authentication Required";
		status[599] = "Network Connect Timeout Error";
	}
	if (status.count(statusCode) == 0)
	{
		return "";
	}
	else
		return status[statusCode];
}

std::string Response::setErrorResponse(int statusCode)
{
	return ("HTTP/1.1 " + std::to_string(statusCode) + " " + statusMessage(statusCode) + "\r\n" );//+ inHeadMethodNameIsHederMapToString((header.getContent())) + "Content-Length: " + std::to_string(getBody().size()) +  "\r\n\n" + getBody());
}

std::string setErrorPage(int statusCode)
{
	return ("<!DOCTYPE html><html><head><body> <h1>" + std::to_string(statusCode) +" </h1></body></html>");
}
std::string Response::writeResponseMessage()
{
	if (statusCode / 100 == 3)
		return ("HTTP/1.1 " + std::to_string(statusCode) + " " + statusMessage(statusCode) + "\r\nServer: jjid\r\nAccept-Ranges: bytes\r\nConnection: keep-alive\r\nContent-Type: text/html;charset=UTF-8\r\nLocation: "+ getBody() +"\r\n\r\n");
	return ("HTTP/1.1 " + std::to_string(statusCode) + " " + statusMessage(statusCode) + "\r\nServer: jjid\r\nAccept-Ranges: bytes\r\nConnection: keep-alive\r\nContent-Type: text/html;charset=UTF-8\r\nContent-Length: " + std::to_string(getBody().size()) +  "\r\n\r\n") ;
}

std::string Response::searchStatusCodeMessage(int statusCode)
{
	(void)statusCode;
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

void Response::setErrStatusCode(int errStatuscode)
{
	this->errStatusCode = errStatuscode;
}

int Response::getStatusCode()
{
	return (statusCode);
}

Header Response::getHeader()
{
	return (header);
}

std::string &Response::getBody()
{
	return (body);
}

int Response::getErrStatusCode()
{
	return (errStatusCode);
}