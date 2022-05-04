#include "Server.hpp"
#include <sys/stat.h>


void Server::setHost(std::vector<std::string> hosts)
{
	this->hosts = hosts;
}

void Server::setPorts(int port)
{
	this->port = port;
}

void Server::setClientBodySize(int clientBodySize)
{
	this->clientBodySize = clientBodySize;
}

// void Server::setLocation(std::vector<Location> locations);
// void Server::setErrPage(std::map<std::vector<int>, std::string> errPage);

std::vector<std::string> Server::getHost()
{
	return (hosts);
}

int Server::getPort()
{
	return (port);
}

int Server::getClientBodySize()
{
	return (clientBodySize);
}

Request& Server::getRequestClass()
{
	return (currRequest);
}

Response& Server::getResponseClass()
{
	return (currResponse);
}

void Server::processMethod()
{
	//allow method확인할것
	//isAllowMethod();
	//405ls
	switch (currRequest.getStartLine().method)
	{
		case GET:
			getMethod();
			break;
		case HEAD:
			headMethod();
			break;
		case POST:
			postMethod();
			break;
		case DELETE:
			deleteMethod();
			break;
		default:
			//405와 같은 에러코드 처리
			break;
	}
}

int checkPath(std::string path)
{
	struct stat buf;

	if (stat(path.c_str(), &buf) == -1)
		return NOT;
	if (S_ISDIR(buf.st_mode)) // 테스트해본결과 /로 끝나던 말던 디렉토리면 걍 디렉토리임
		return DIR;
	else if (S_ISREG(buf.st_mode))
		return FILE;
	// else if (S_ISLNK())
	// 	return LINK;
	else
		return (-1);
}

void Server::getMethod()
{
	std::string path = this->currRequest.getStartLine().path;
	
	// 파일 디렉토리 링크 등으로 뱉어낼거임
	//디렉토리 -> 인덱스파일 탐색
	//파일 -> 파일 오픈 -> 오픈 리턴으로 실패확인가능 . ngx_de_access라는 엔진엑스 매크로함수 처럼 권한 체크할지는 추후판단
	//링크는 어떻게 해야할까(링크타고 찾아간다 등) 일단 보류
	switch (checkPath(path))
	{
		case DIR :
			/* code */
			break;
		case FILE :
			/* code */
			break;
		case NOT :
			/* code */
			break;
		default:
			break;
	}

}

void Server::postMethod()
{
	
}

void Server::deleteMethod()
{
		
}

void Server::headMethod()
{
		
}
