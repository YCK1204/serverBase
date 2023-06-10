#include "../header/Http.hpp"

void	Http::occurException(const int &line, const std::string &msg, exception type, files file, const std::string &reason) {
	std::string cpp;
	switch (file) {
		case F_HTTP    : cpp = "Http"            ; break ;
		case PARSING   : cpp = "parsing"         ; break ;
		case S_PARSING : cpp = "ServerParsing"   ; break ;
		case L_PARSING : cpp = "LocationParsing" ; break ;
		case UTIL      : cpp = "util"            ; break ;
	}
	exception_util(cpp + ".cpp <Line " + ft_to_string(line) + ">, " + msg + " : " + reason, type);
}

void    Http::exception_util(const std::string &msg, exception type) {
	std::cerr << msg << std::endl;

	if (type == CONFIG)
		throw NotValidConfigFileException();
	else if (type == FILEROOT)
		throw NoSuchFileException();
	else if (type == EMPTY)
		throw EmptyFileException();
	else if (type == PORT)
		throw ServerPortOverlapException();
	else if (type == ROOT)
		throw LocationRootOverlapException();
	else if (type == ADDR)
		throw notValidAddrException();
	else if (type == HTTP)
		throw SettingHttpException();
	else if (type == SERVER)
		throw RunServerException();
}

void    Http::checkErrnoSocket(const int line) {

}

void    Http::checkErrnoBind(const int line) {

}

void    Http::checkErrnoListen(const int line) {

}

void    Http::checkErrnoAccept(const int line) {

}

void	Http::checkErrnoSetSocketOpt(const int line) {

}

void	Http::checkErrnoSetEvset(const int line) {

}

void	Http::checkErrnoSetKqueue(const int line) {

}

void	Http::checkErrnoSetKevent(const int line) {

}

const char *Http::NotValidConfigFileException::what() const throw() { return ("Error : Not Valid Configuration File");}
const char *Http::EmptyFileException::what() const throw() { return ("Error : Empty File");}
const char *Http::ServerPortOverlapException::what() const throw() { return ("Error : Server Port Overlap");}
const char *Http::LocationRootOverlapException::what() const throw() { return ("Error : Location Block Default Root Overlap");}
const char *Http::NoSuchFileException::what() const throw() { return ("Error : No Such File");}
const char *Http::notValidAddrException::what() const throw() { return ("Error : Is Not Valid Address");}
const char *Http::SettingHttpException::what() const throw() { return ("Error : Fail Server Setting function");}
const char *Http::RunServerException::what() const throw() { return ("Error : Occured Exception During Run Server");}