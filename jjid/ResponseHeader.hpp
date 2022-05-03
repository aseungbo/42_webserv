#ifndef RESPONSEHEADER_HPP
#define RESPONSEHEADER_HPP

#include "uniHeader.hpp"
#include "Header.hpp"

class ResponseHeader : public Header
	{
		private:
			std::string server;
			std::vector<std::string> allow;
			std::string location;
		public:
		
	};
#endif