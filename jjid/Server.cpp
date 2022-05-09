#include "Server.hpp"

void Server::setHost(std::vector<std::string> hosts)
{
	this->hosts = hosts;
}

void Server::setPort(int port)
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
	//allow method확인할것 // -> 메소드 함수 안에서 로케이션 정보 있이 하거나, 여기서 로케이션 결정후 확인하거나
	//isAllowMethod();
	//405ls
	switch (currRequest.getStartLine().method)
	{
		case GET:
			getMethod(NO_HEAD);
			break;
		case HEAD:
			getMethod(YES_HEAD);
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

Location Server::getDefaultLocation()
{
	Location defaultLocation;
	//TODO
	return defaultLocation;
}

Location Server::whereIsLocation(std::string path, int pathType, std::vector<Location> locations)
{
//테스트 해보니까 로케이션이 없는경우 기본값 로케이션으로 하나봄 -> 메서드로 기본값로케이션 반환하는 거 하나 만들어서 (로케이션 사이즈가 0이거나 일치하는 로케이션이 없을때)에 예외처리 ㄱ
	if (locations.size() == 0 )//|| pathType == )
		return (getDefaultLocation());
	//형식 맞춰줌 인덱싱으로 연산 빠를 거라고 예상...	
	if (path[path.length() - 1] != '/')
			path += '/';
	//반복문 돌면서 일치하는거 확인
	for (int idx = 0 ; idx < locations.size(); idx++)
	{
		std::string locationPath = locations[idx].getRoot();
		if (locationPath[locationPath.length() - 1] != '/')
			locationPath += '/';
		if (locationPath == path)
			return (locations[idx]);
	}
	return (getDefaultLocation());
};

void Server::serchIndex(std::string &path, Location currLocation)
{
	struct stat buf;
	
	//설정 인덱스가 없으면 디폴트 인덱스(현재는 index.html파일) 추가 후 종료
	if (currLocation.getIndex().size() == 0)
	{
		path += DEFAULT_INDEX;
		return ;
	}
	//설정 인덱스가 있다면 가장 처음 있는 파일 경로 넣고 종료
	std::vector<std::string> currIndex = currLocation.getIndex();
	for (int idx = 0; idx < currIndex.size(); idx++)
	{
		std::string tryIndexPath = path + currIndex[idx];
		if (stat((tryIndexPath).c_str(), &buf) == 0)
		{
			path = tryIndexPath;
			return ;
		}
	}
}

void Server::openFile(std::string path, int isHead)
{
	std::cout << "\nopen call \n";
	// 파일 읽기 준비
	std::ifstream in(path);
	std::string body;
	if (in.is_open()) {
		if (isHead == NO_HEAD)
		{
		// 위치 지정자를 파일 끝으로 옮긴다.
		in.seekg(0, std::ios::end);
		// 그리고 그 위치를 읽는다. (파일의 크기)
		int size = in.tellg();
		// 그 크기의 문자열을 할당한다.
		body.resize(size);
		// 위치 지정자를 다시 파일 맨 앞으로 옮긴다.
		in.seekg(0, std::ios::beg);
		// 파일 전체 내용을 읽어서 문자열에 저장한다.
		in.read(&body[0], size);
		
		// std::cout << body << std::endl;
		}
	}
	else {
		std::cout << "파일을 찾을 수 없습니다!" << std::endl;
		in.close();
		// throw (404);//catch해서 set다 호출할것 TODO
		return;
	}
	this->currResponse.setStatusCode(200);
	this->currResponse.setBody(body);
	
	std::cout << "whereis test::::\n";
	std::cout << "test:::: " << currResponse.writeResponseMessage()<<std::endl;
	in.close();
	return ;
}

void Server::getMethod(int isHead)
{
	std::string path = this->currRequest.getStartLine().path;
	path = "." + path;//root 키워드로 설정하기 설정 없다면 디폴트로 추가하기, 절대경로로 바꿀것
	int pathType = checkPath(path);
	std::cout << path <<std::endl;
	Location currLocation = whereIsLocation(path, pathType, locations);//find or  map match 등 다른이름 추천받음
	
	// 파일 디렉토리 링크 등으로 뱉어낼거임
	//디렉토리 -> 인덱스파일 탐색
	//파일 -> 파일 오픈 -> 오픈 리턴으로 실패확인가능 . ngx_de_access라는 엔진엑스 매크로함수 처럼 권한 체크할지는 추후판단
	//링크는 어떻게 해야할까(링크타고 찾아간다 등) 일단 보류
	
	//설정한 인덱스가 디렉토리인 경우를 생각했을때 실제 탐색해볼 주소를 완성하기 위한 serchIndex(처리될 로케이션의 정보를 가지고 Path를 완성시킴)
	// if (checkPath(path) == DIR)
	switch (pathType)
	{
		case DIR ://디렉토리 안에 설정된 인덱스 파일들 탐색 해볼것임 ,  인덱스 파일 없다면(권한없어도) 403 // 만약 설정된 인덱스가 두개 이상이라면 첫번째꺼 // 만약 설정이 없다면 기본적으로 index.html 을 탐색함
			serchIndex(path, currLocation);
			// break;// 브레이크 안걸면 밑에거 까지 실행해주는걸로 아는디 불안하면 그냥 opneFile호출하시오
		case FILE ://해당파일찾아볼것 마찬가지로 없다면 403
			openFile(path, isHead);
			break;
		case NOT ://404
			/* throw 404 error */
			break;
		default:
			break;
	}

}

void Server::postMethod()
{
	// 기존 파일이 있는지 확인 후 있으면 수정, 없으면 크리에이트 , 디렉토리면...?일단 에러
	// 쿼리날리는거, 디비접근 등도 고려해야하는지 섭젝 읽어볼것
	
}

void Server::deleteMethod()
{
		
}

// void Server::headMethod()
// {
		
// }
