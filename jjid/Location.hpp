#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "uniHeader.hpp"
# include "Server.hpp"

class Location : public Server
{
	private:
		std::string root;
		std::string index;
		// bool autoIndex;
		// std::map<std::vector<int>, std::string> errPage;
		std::vector<std::string> methodsAllowed;
	public: 
		// void
};

#endif