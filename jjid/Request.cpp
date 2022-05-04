#include "Request.hpp"

void RequestHeader::setHost(std::string host)
{
	this->host = host;
}

std::vector<std::string> Request::splitRequestMessage(std::string str, char delimiter)
{
    std::istringstream iss(str);              // istringstream에 str을 담는다.
    std::string buffer;                      // 구분자를 기준으로 절삭된 문자열이 담겨지는 버퍼
    std::vector<std::string> result;
    // istringstream은 istream을 상속받으므로 getline을 사용할 수 있다.
    while (std::getline(iss, buffer, delimiter))
        result.push_back(buffer);               // 절삭된 문자열을 vector에 저장
    return result;
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
	return (-1);
	// 	throw 지원하지 않는 메소드
}

void Request::initStartLine(const std::string &str)
{
	std::vector<std::string> parseStartLine = splitRequestMessage(str , ' ');


	// for (std::vector<std::string>::iterator iter = parseStartLine.begin(); iter != parseStartLine.end(); iter++)
	// 	std::cout << "[ ] : " << *iter << std::endl;
	// std::cout << parseStartLine.size() << std::endl;
	
	try
	{
		if (parseStartLine.size() != 3)
			throw (StartLineErr());
		
		startline.method = methodToNum(parseStartLine[0]);
		startline.path = parseStartLine[1];
		startline.http = parseStartLine[2];
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
}

void Request::parseRequestMessage(std::string requestMessage)
{
	std::vector<std::string> parseRequest = splitRequestMessage(requestMessage, '\n');
	// msg parsing
	//맨마지막은 '\n'
	int i = 0;
	initStartLine(parseRequest[i++]);

	std::cout << startline.method << " " << startline.path << " " << startline.http << std::endl;
	std::cout << std::endl;

	
	//test//
	// for (std::vector<std::string>::iterator iter = parseRequest.begin(); iter != parseRequest.end(); iter++)
	// 	std::cout << "[ ] : " << *iter << std::endl;



	// startline

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