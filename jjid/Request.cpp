#include "Request.hpp"

std::vector<std::string> Request::splitRequestMessage(std::string str, char delimiter)
{
    std::istringstream iss(str);
    std::string buffer;
    std::vector<std::string> result;
    while (std::getline(iss, buffer, delimiter))
    {
		result.push_back(buffer);
	}
    return result;
}

void Request::addBody(std::string& str)
{
	body.append(str);
}

void Request::setBody(std::string str)
{
	body = str;
}

void Request::setPath(std::string str)
{
	startline.path = str;
}

int Request::methodToNum(const std::string& str)
{
	if (str == "GET")
		return (0);
	else if (str == "HEAD")
		return (1);
	else if (str == "POST")
		return (2);
	else if (str == "DELETE")
		return (3);
	else if (str == "PUT")
		return (4);
	return (-1);
}

void Request::initStartLine(const std::string &str)
{
	std::vector<std::string> parseStartLine = splitRequestMessage(str , ' ');

	try
	{
		if (parseStartLine.size() != 3)
			throw (StartLineErr());
		
		startline.method = methodToNum(parseStartLine[0]);
		startline.path = parseStartLine[1];
		startline.http = parseStartLine[2];

		std::vector<std::string> parsePath = splitRequestMessage(parseStartLine[1] , '.');
		
		if (parsePath.size() == 1)
			extension = "";
		else
			extension = parsePath[1];
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

std::pair <std::string, std::string> Request::initRequestHeader(const std::string header)
{
	std::vector<std::string> splitHeader = splitRequestMessage(header, ':');
	
	if (splitHeader[0].size() == 0)
	{
		this->startline.method = 400;
		throw( StartLineErr() );
	}
	if (splitHeader.size() > 2)
	{
		for (unsigned long i = 2; i < splitHeader.size(); i++)
			splitHeader[1] += (":" + splitHeader[i]);
	}
	std::pair<std::string, std::string> temp;
	temp.first = splitHeader[0];
	temp.second = splitHeader[1].substr(1,splitHeader[1].size() - 2);
	return (temp);
}


void Request::clearRequest()
{
	extension.clear();
	startline.http.clear();
	startline.method = 0;
	startline.path.clear();
	body.clear();
	cgi = 0;
	header.getContent().clear();
}
void Request::parseRequestMessage(std::string requestMessage)
{
	if (requestMessage.size() == 0)
		return ;
	clearRequest();

		std::vector<std::string> parseRequest = splitRequestMessage(requestMessage, '\n');
		if (parseRequest.size() > 0)
			initStartLine(parseRequest[0]);	
		else
			return (printErr("no requset"));
		std::vector<std::string>::iterator iter = parseRequest.begin() + 1;
	try
	{
		for (; (*iter) != "\r" && iter != parseRequest.end(); iter++)
			header.getContent().insert(initRequestHeader(*iter));
		if (requestMessage.size() >= requestMessage.find("\r\n\r\n") + 4)
			body = requestMessage.substr(requestMessage.find("\r\n\r\n") + 4);
	}
	catch( std::exception e)
	{
		return ;
	}
}

t_StartLine Request::getStartLine()
{
	return (startline);
}

Header& Request::getHeader()
{
	return (header);
}

std::string& Request::getBody()
{
	return (body);
}

std::string Request::getExtension()
{
	return (extension);
}