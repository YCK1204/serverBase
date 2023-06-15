#include "../header/Http.hpp"

std::string Http::buildHtml(const std::string msg) {
	std::string ret;

	ret += "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "    <meta charset=\"UTF-8\">\n"
            "    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=chrome\">\n"
            "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
            "    <title>Document</title>\n"
            "</head>\n"
            "<body>\n";
	ret += "    " + msg;
	ret += "</body>\n"
           "</html>\r\n\r\n";
	return ret;
}

std::string Http::buildErrorHtml(const int status) {
    std::string ret;
    std::string msg;

    msg = "<h1> " + ft_to_string(status) + "에러났죠? ㅋㅋ <br>그렇게 하는거 아닌데 ㅋ</h1>\n";
    ret = buildHtml(msg);
    return ret;
}

std::string Http::buildErrorMsg(int clnt_sock) {
    std::string ret;

    ret += "HTTP/1.1 " + ft_to_string(err) + " "+ mime.getStatus(err) + "\r\n";
    ret += "Connection: keep-alive\r\n";
    ret += getDate() + "\r\n";
    ret += "Content-type: text/html\r\n";

    return ret;
}