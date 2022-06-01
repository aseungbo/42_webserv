#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "uniHeader.hpp"
// # include "Server.hpp"

class Location
	{
		private:
			std::string root;
			std::string path;
			std::vector<std::string> index;
			std::map<std::vector<int>, std::string> errPage;
			std::vector<std::string> AllowMethod;
			bool autoIndex;
			int locationType;
			std::string extension;
			std::string cgiPath;
			int returnCode;
			std::string returnUrl;
			
		public:
			std::string getRoot();
			std::string getPath();
			std::vector<std::string> getIndex();
			std::map<std::vector<int>, std::string> getErrPage();
			std::vector<std::string> getAllowMethod();
			int getLocationType();
			std::string getExtension();
			std::string getCgiPath();
			bool getAutoIndex();
			
			int getReturnCode();
			std::string getReturnUrl();

			void setRoot(std::string root);
			void setPath(std::string path);
			void setIndex(std::vector<std::string> index);
			void setErrPage(std::map<std::vector<int>, std::string> errPage);
			void setAllowMethod(std::vector<std::string> AllowMethod);
			void setLocationType(int type);
			void setExtension(std::string extension);
			void setCgiPath(std::string cgiPath);
			
			void setReturnCode(int code);
			void setReturnUrl(std::string url);
			
			void setAutoIndex(bool flag);
	};

#endif