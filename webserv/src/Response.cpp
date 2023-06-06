#include "../header/Http.hpp"

std::string Http::setResponseLine(LocationBlock &location, ServerBlock &server, size_t const &ResponseCode, std::string msg) {
    std::string ret = "HTTP/1.1 ";
    std::string tmp;
    std::stringstream ss;

    ss << ResponseCode;
    ss >> tmp;
    ret += tmp + " " + msg + "\r\n"
           "Server:" + server.server_name + "\r\n";
    ss << BUF_SIZE;
    ss >> tmp;
    ret += "Content-lenght:" + tmp + "\r\n";
    if (ResponseCode == 404 || location.index_root.find(".html", 1) != std::string::npos)
        ret += "Content-type:text/html\r\n\r\n";
    else
        ret += "Content-type:text/plain\r\n\r\n";
    return ret;
}

std::string Http::checkValidRequestLine(std::string &method, std::string &root, std::string &http_ver, std::string &temp, ServerBlock &server, std::vector<std::pair<std::string, LocationBlock> >::iterator it) {
    if (method.compare("GET") && method.compare("POST") && method.compare("DELETE"))
    {
        std::cout << "first" << std::endl;
        return setResponseLine(it->second, server, 
            501, "Not Implemented"
        );
    }
    else if (it != server.location_block.end() && (!method.compare("GET") && !it->second.methods[GET]) || (!method.compare("POST") && !it->second.methods[POST])\
                || (!method.compare("DELETE") && !it->second.methods[DELETE]))
                {
                    std::cout << "second" << std::endl;
        return setResponseLine(it->second, server, 
            501, "Not Implemented"
        );
                }
    else if (it == server.location_block.end())
        return setResponseLine(it->second, server, 
            404, "Not Found"
        );
    else if (root.length() >= 30)
        return setResponseLine(it->second, server, 
            414, "Request-URI Too Long"
        );
    else if (http_ver.substr(0, 4).compare("HTTP"))
    {
        std::cout << "third" << std::endl;
        return setResponseLine(it->second, server, 
            501, "Not Implemented"
        );
    }
    else if (http_ver.substr(5).compare("1.1"))
        return setResponseLine(it->second, server, 
            505, "HTTP Version Not Supported"
        );
    else if (!temp.empty())
        return setResponseLine(it->second, server, 
            400, "Bad Request"
        );
    else if (it->second.ret)
        return setResponseLine(it->second, server, 
            301, "Moved Permanently"
        );
    return setResponseLine(it->second, server, 200, "OK");
}

std::string Http::extractResponseCode(std::string msg) {
    int i;
    for (i = 0; msg[i]; i++){
        if ('1' <= msg[i] && msg[i] <= '5')
            break ;
    }
    return msg.substr(i, 3);
}

std::string Http::readFile(ServerBlock &server, LocationBlock &location, std::string &msg) {
    std::string line, ret;
    std::ifstream file;

    if (msg.find("20", 5) != std::string::npos) {
        file.open(location.index_root);
        while (std::getline(file, line)){
            ret += line;
            ret += "\n";
        }
    }
    else {
        ret += "<!DOCTYPE html>\n"
               "<html>\n"
               "<head>\n"
               "    <meta charset=\"UTF-8\">\n"
               "    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=chrome\">\n"
               "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
               "    <title>Document</title>\n"
               "</head>\n"
               "<body>\n";
        ret += "<h1>" + msg.substr(9, 3) + "</h1>\n"
                "</body>\n"
                "</html>";
    }
    return ret;
}

std::pair<std::string, std::string> Http::makeResponse(ServerBlock &server, char *msg) {
    std::pair<std::string, std::string>   ret;
    std::string method, root, http_ver, temp;
    std::vector<std::pair<std::string, LocationBlock> >::iterator it;
    std::stringstream ss, s;

    std::cout << "msg : " << msg << std::endl;
    ss << msg;
    std::string tmp;
    std::getline(ss, tmp);
    s << tmp;
    s >> method >> root >> http_ver >> temp;

    for (it = server.location_block.begin(); it != server.location_block.end() && root.compare(it->second.default_root); it++);
    ret.first = checkValidRequestLine(method, root, http_ver, temp, server, it);
    ret.second = readFile(server, it->second, ret.first);
    return (ret);
}