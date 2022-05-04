#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "uniHeader.hpp"
# include "RequestHeader.hpp"
# include <sstream> // istringstream
	
typedef struct s_StartLine
{
	int method;
	std::string http;
	std::string path;
} t_StartLine;
	
class Request
{
	private:
		t_StartLine startline;
		RequestHeader header;                                
		std::string body;
		bool cgi;//해당 요청(확장자 보고 판단 )이 cgi인지
		
	public:
		void parseRequestMessage(std::string requestMessage);
		void initStartLine(const std::string &str);
		int methodToNum(const std::string& str);

		std::vector<std::string> splitRequestMessage(std::string str, char delimiter);

		t_StartLine getStartLine();
		RequestHeader& getRequestHeader();
		std::string getBody();

		class StartLineErr : public std::exception
		{
			const char *what() const throw ()
			{
				return ("StartLine Err\n");
			}
		};
};

#endif