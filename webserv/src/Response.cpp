#include "../header/Http.hpp"

std::string Http::setResponseLine(LocationBlock &location, size_t const &ResponseCode) {

}

std::string Http::checkValidRequestLine(std::string &method, std::string &root, std::string &http_ver, std::string &temp, ServerBlock &server, std::vector<std::pair<std::string, LocationBlock> >::iterator it) {
    std::string ret;

    if (method.compare("GET") || method.compare("POST") || method.compare("DELETE"))
        return setResponseLine(it->second,
            501
        );
    else if (it != server.location_block.end() && (!method.compare("GET") && !it->second.methods[GET]) || (!method.compare("POST") && !it->second.methods[POST])\
                || (!method.compare("DELETE") && !it->second.methods[DELETE]))
        return setResponseLine(it->second,
            501
        );
    else if (it == server.location_block.end())
        return setResponseLine(it->second,
            404
        );
    else if (root.length() >= 30)
        return setResponseLine(it->second,
            414
        );
    else if (http_ver.substr(0, 4).compare("HTTP"))
        return setResponseLine(it->second,
            501
        );
    else if (http_ver.substr(5).compare("1.1"))
        return setResponseLine(it->second,
            505
        );
    else if (!temp.empty())
        return setResponseLine(it->second,
            400
        );
    else if (it->second.ret)
        return setResponseLine(it->second,
            301
        );
}

std::vector<std::pair<std::string, std::string> > Http::makeResponse(ServerBlock &server, std::stringstream &ss) {
    std::vector<std::pair<std::string, std::string> >   ret;
    std::string method, root, http_ver, temp;
    std::vector<std::pair<std::string, LocationBlock> >::iterator it;

    ss >> method >> root >> http_ver >> temp;

    for (it = server.location_block.begin(); it != server.location_block.end() && root.compare(it->second.default_root); it++);
    ret[0].first = checkValidRequestLine(method, root, http_ver, temp, server, it);
    return (ret);
}