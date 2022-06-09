#include "Request.hpp"

std::vector<std::string> Request::splitRequestMessage(std::string str, char delimiter)
{
    std::istringstream iss(str);              // istringstream에 str을 담는다.
    std::string buffer;                      // 구분자를 기준으로 절삭된 문자열이 담겨지는 버퍼
    std::vector<std::string> result;
    // istringstream은 istream을 상속받으므로 getline을 사용할 수 있다.
    while (std::getline(iss, buffer, delimiter))
    {
        result.push_back(buffer);               // 절삭된 문자열을 vector에 저장
	}
	
	// result.push_back("\n");
	// std::cout <<  result[result.size()-1] << std::endl;
	// printf("d : %d \n", result[result.size()-1][0] );
	// printf("size : %d \n", (int)result[result.size()-1].size() );
    return result;
}

void Request::addBody(std::string& str)
{
	body.append(str);
	// body += str;
}

void Request::setBody(std::string str)
{
	body = str;
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

	try
	{
		if (parseStartLine.size() != 3)
			throw (StartLineErr());
		
		startline.method = methodToNum(parseStartLine[0]);
		startline.path = parseStartLine[1];
		startline.http = parseStartLine[2];

		std::vector<std::string> parsePath = splitRequestMessage(parseStartLine[1] , '.');
		
		if (parsePath.size() == 1)
			extension = "";  //나중에 뭐 넣을 지 다시 고민
		else
			extension = parsePath[1]; // parsePath[parsePath.size() - 1]; >> 이렇게 마지막것 만 가져오기
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

std::pair <std::string, std::string> Request::initRequestHeader(const std::string header)
{

	// std::cout << header << std::endl;
	std::vector<std::string> splitHeader = splitRequestMessage(header, ':');
	
	if (splitHeader.size() > 2)
	{
		for (int i = 2; i < splitHeader.size(); i++)
			splitHeader[1] += (":" + splitHeader[i]);
	}
	std::pair<std::string, std::string> temp;
	temp.first = splitHeader[0];
	temp.second = splitHeader[1].substr(1,splitHeader[1].size() - 2);//TODO:공백날리기 다시생각 하기~ 스페이스 여러개 또는 없이 왔을때
	
	std::cout << temp.first <<  temp.second << std::endl;
	std::cout << "initRequestHeader : " << "[" << temp.first << " , "<< temp.second << "]" << std::endl;

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
	std::cout <<"<" << requestMessage << ">"<<std::endl ;
	// std::cout << requestMessage << std::endl;
	// if (headerDone == false)
	// {
		std::vector<std::string> parseRequest = splitRequestMessage(requestMessage, '\n');
		//맨마지막은 '\n'
		if (parseRequest.size() > 0)
			initStartLine(parseRequest[0]);	
		else
			return (printErr("no requset"));
		std::vector<std::string>::iterator iter = parseRequest.begin() + 1; // jji : 찝찝
		for (; (*iter) != "\r" && iter != parseRequest.end(); iter++)
			header.getContent().insert(initRequestHeader(*iter));
		if (requestMessage.size() >= requestMessage.find("\r\n\r\n") + 4)
			body = requestMessage.substr(requestMessage.find("\r\n\r\n") + 4);
		std::cout << "in parse body:" << body <<std::endl;
		std::cout << "in parse getbody:" << getBody() <<std::endl;
		
		// for (; iter != parseRequest.end(); iter++)
		// 	if (iter != parseRequest.end() -1 )
		// 		body += (*iter + "\n");
		// 	else
		// 		body += (*iter);
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

//static 변수는 class 끼리 공유하는 변수니까
// init함수로 초기화를 하는 느낌으로 해야될듯
// 생성자에서 해버리면 다른 친구들도 해주어야 하는데 문제가 상당히 생길 듯
//class Server 안에 std::map<std::string, std::string > mimeType이라는 변수를 저장하고
// std::string을 반환하게 끔
// if (mimeType.size() == 0)
// {
//  mimeType["abc"] = "fdafad";
// }
//	
// if (못찾았을 때)
// 		return "text/plain";
// else
// 		return mimeType[extension]; 키에 대한 값을 반환
//