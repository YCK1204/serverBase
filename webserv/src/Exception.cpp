#include "../header/Http.hpp"

void	Http::occurException(const int &line, const std::string &msg, exception type, files file, const std::string &reason) {
	std::string cpp;
	switch (file) {
		case F_HTTP    : cpp = "Http"            ; break ;
		case PARSING   : cpp = "parsing"         ; break ;
		case S_PARSING : cpp = "ServerParsing"   ; break ;
		case L_PARSING : cpp = "LocationParsing" ; break ;
		case UTIL      : cpp = "util"            ; break ;
		case CLIENT    : cpp = "Client"			 ; break ;
	}
	exception_util(cpp + ".cpp <Line " + ft_to_string(line) + ">, " + msg + " : " + reason, type);
}

void    Http::exception_util(const std::string &msg, exception type) {
	std::cerr << msg << std::endl;

	switch (type) {
		case HTTP		: throw SettingHttpException();
		case ROOT		: throw LocationRootOverlapException();
		case PORT		: throw ServerPortOverlapException();
		case ADDR		: throw notValidAddrException();
		case EMPTY		: throw EmptyFileException();
		case SERVER		: throw RunServerException();
		case CONFIG		: throw NotValidConfigFileException();
		case FILEROOT	: throw NoSuchFileException();
	}
}

void	Http::serverFunctionExecuteFailed(const int line, std::string msg) {
	occurException(line, msg, HTTP, F_HTTP,
	"[" + static_cast<const std::string>(strerror(errno)) + "]");
}

void	Http::serverFunctionExecuteFailed(const int line, std::string msg, std::string detail) {
	occurException(line, msg, HTTP, F_HTTP, detail);
}

const char *Http::EmptyFileException::				what() const throw() { return ("Error : Empty File");}
const char *Http::NoSuchFileException::				what() const throw() { return ("Error : No Such File");}
const char *Http::ServerPortOverlapException::		what() const throw() { return ("Error : Server Port Overlap");}
const char *Http::notValidAddrException::			what() const throw() { return ("Error : Is Not Valid Address");}
const char *Http::NotValidConfigFileException::		what() const throw() { return ("Error : Not Valid Configuration File");}
const char *Http::SettingHttpException::			what() const throw() { return ("Error : Fail Server Setting function");}
const char *Http::LocationRootOverlapException::	what() const throw() { return ("Error : Location Block Default Root Overlap");}
const char *Http::RunServerException::				what() const throw() { return ("Error : Occured Exception During Run Server");}