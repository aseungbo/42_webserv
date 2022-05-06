#ifndef REQUESTHEADER_HPP
# define REQUESTHEADER_HPP

# include "uniHeader.hpp"
# include "Header.hpp"

class RequestHeader : public Header
{
	private:
		std::string host;

		// 커넥션
		//
		// host 서ㅓㅂ의 도메인 네임
		// User-Agent 어떤 클라이언트를 통해 요청
		// Authotization 인증
					// Cookie
					// Origin
		// IF-Modified-Since (조건부 요펑이 들어오면 처리해야 할 것)
	public:
		void setHost(std::string host);
};

#endif