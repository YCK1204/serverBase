#include "../header/Http.hpp"
// /* line */
// std::string Http::getMethod(std::string req_msg) {
//     std::stringstream ss(req_msg);
//     std::string ret;

//     ss >> ret;
//     return ret;
// }

// std::string Http::getRoot(std::string req_msg) {
//     std::stringstream ss(req_msg);
//     std::string tmp, ret;

//     ss >> tmp >> ret;
//     return ret;
// }

// std::string Http::getHTTP(std::string req_msg) {
//     std::stringstream ss(req_msg);
//     std::string t, tt, ret;

//     ss >> t >> tt >> ret;
//     if (ret.find("HTTP") != std::string::npos)
//         ret.substr(0, 4);
//     return ret;
// }

// std::string Http::getHttpVer(std::string req_msg) {
//     std::stringstream ss(req_msg);
//     std::string t, tt, ret;
//     size_t  len = 0;

//     ss >> t >> tt >> ret;
//     if ((len = ret.find("/")) != std::string::npos)
//         ret.substr(len + 1);
//     return ret;
// }

// // std::string Http::getResponseLine(std::string req_msg) {
    
// // }
// /* line */

// /* header */
// std::string Http::getHost(std::string req_msg) {
//     std::stringstream   ss(req_msg);
//     std::string         line, ret, cmp;
//     size_t              len;

//     cmp = "HOST:";
//     while (std::getline(ss, line)) {
//         if (line.find(cmp))
//             break ;
//         line.clear();
//     }
//     if (std::strncmp(line.c_str(), cmp.c_str(), cmp.length()))
//         return (ret = "65536");
//     len = line.rfind(":");
//     if (len == std::string::npos)
//         return (ret = "65536");
//     ret = line.substr(len + 1);
//     return ret;
// }

// std::string Http::getConnection(std::string req_msg) {
//     std::stringstream   ss(req_msg);
//     std::string         line, ret, cmp;
//     size_t              len;

//     cmp = "Connection:";
//     while (std::getline(ss, line)) {
//         if (line.find(cmp))
//             break ;
//         line.clear();
//     }
//     if (std::strncmp(line.c_str(), cmp.c_str(), cmp.length()))
//         return (ret = "");
//     len = line.rfind(":");
//     if (len == std::string::npos)
//         return (ret = "0");
//     ret = line.substr(len + 1);
//     return ret;
// }

// // std::string Http::getResponseHeader(std::string req_msg) {

// // }
// // /* header */

// // std::string Http::getResponseBody(std::string req_msg) {

// // }

// // std::string Http::getMsg(int clnt_sock) {
// //     std::string ret, req_msg = clients[clnt_sock].request;
    
// // }

// // std::string Http::getContent(int clnt_sock) {

// // }

// std::pair<std::string, std::string> Http::getResponse(int clnt_sock) {
//     std::string req_msg = clients[clnt_sock].request;
//     std::string method, root, http, ver, ret, host, connection;
//     std::pair<std::string, std::string> ret;
    
//     method     = getMethod(req_msg);
//     root       = getRoot(req_msg);
//     http       = getHTTP(req_msg);
//     ver        = getHttpVer(req_msg);
//     host       = getHost(req_msg);
//     connection = getConnection(req_msg);
//     checkRequestMsg(method, root, http, ver, host, connection);

//     if (err)
//         ret.first = buildErrorMsg(clnt_sock);

//     else {
//     //    err.first = getResponseLine(clnt_sock, );

//     }
// }

// ServerBlock Http::getServer(std::string host, std::string root) {
//     int     port = ft_stoi(host);
//     bool    f = false;
//     std::vector<ServerBlock>::iterator it;

//     for (it = this->server.begin(); it != this->server.end(); it++) {
//         ServerBlock &server = *it;
//         if (server.port == port) { // 포트가 같을 때
//             for (std::vector<LocationBlock>::iterator it = server.location.begin(); it != server.location.end(); it++) {
//                 LocationBlock &location = *it;
//                 if (!location.default_root.compare(root)) { // 루트 발견
//                     f = true;
//                     break ;
//                 }
//             }
//         }
//         if (f)
//             break ;
//     }
//     if (it == this->server.end()) { // 루트가 없을 때
//         err = 404;
//         for (it = this->server.begin(); it != this->server.end(); it++) {
//             ServerBlock &server = *it;
//             if (server.port == port)
//                 break ;
//         }
//     }

//     if (it == this->server.end())  // 포트가 이상할 때
//         return *(this->server.begin());
//     return *it;
// }

// LocationBlock    Http::getLocation(std::string root, ServerBlock server) {
//     std::vector<LocationBlock>::iterator it;

//     for (it = server.location.begin(); it != server.location.end(); it++) {
//         LocationBlock   &location = *it;
//         if (!location.default_root.compare(root))
//             break ;
//     }

//     if (it == server.location.end())
//         return *(server.location.begin());
//     return *it;
// }

// void    Http::checkRequestMsg(std::string method, std::string root, std::string http, std::string ver, std::string host, std::string connection) {
//     ServerBlock server = getServer(host, root);
//     LocationBlock location = getLocation(root, server);
//     bool    methods[3] = {location.methods[0], location.methods[1], location.methods[2]};

//     if (method.compare("GET") && method.compare("POST") && method.compare("DELETE"))
//         err = 400;
//     else if (!err && !methods[GET] && !method.compare("GET") && !methods[POST] && !method.compare("POST") && !methods[DELETE] && !method.compare("DELETE"))
//         err = 405;
//     else if (!err){
//         if (location.default_root.compare(root))
//             err = 404;
//         else if (http.compare("HTTP"))
//             err = 501;
//         else if (!host.compare("65536") || server.port != ft_stoi(host))
//             err = 403;
//         else if (connection.compare("keep-alive") && connection.compare("close"))
//             err = 400;
//     }
// }

std::string Http::buildErrorMsg(int clnt_sock) {
    std::string ret;

    ret += "HTTP/1.1 " + ft_to_string(err) + " "+ mime.getStatus(err) + "\r\n";
    std::cout << ret << std::endl;
    ret += "Connection: keep-avlive\r\n";
    ret += getDate() + "\r\n";
    ret += "Content-type: text/html\r\n";

    return ret;
}



/*
std::string Http::setResponseLine(std::vector<std::pair<std::string, LocationBlock> >::iterator &location, std::vector<std::pair<unsigned short, ServerBlock> >::iterator server, size_t const &ResponseCode, std::string msg) {
    std::string ret = "HTTP/1.1 ";
    std::string tmp;
    std::stringstream ss;

    ss << ResponseCode;
    ss >> tmp;
    ret += tmp + " " + msg + "\r\n";
    ss << BUF_SIZE;
    ss >> tmp;
    if (server != this->server_block.end()) {
        ret += "Server:" + server->second.server_name + "\r\n";
        if (location != server->second.location_block.end()) {
            if (location->second.cgi_bin.empty() || location->second.index_root.find(".html", 1) != std::string::npos)
                ret += "Content-type:text/html\r\n";
            else
                ret += "Content-type:text/plain\r\n";
            if (location->second.ret)
                ret += "Location:" + location->second.redirect + "\r\n";
        }
        else
            ret += "Content-type:text/html\r\n";
    }
    return ret;
}

std::string Http::checkValidRequestLine(std::string &method, std::string &root, std::string &http_ver, std::string &temp, std::vector<std::pair<unsigned short, ServerBlock> >::iterator it, std::vector<std::pair<std::string, LocationBlock> >::iterator itt) {
    // 경로의 길이가 100 이상일 때
    if (root.length() >= 100)
        return setResponseLine(itt, it, 
            414, "Request-URI Too Long"
        );
    // 없는 포트
    else if (it == this->server_block.end())
        return setResponseLine(itt, it,
            403, "Forbidden"
        );
    // 없는 메소드
    else if (method.compare("GET") && method.compare("POST") && method.compare("DELETE"))
        return setResponseLine(itt, it, 
            405, "Method not allowed"
        );
    // 해당 로케이션 경로가 없을 때
    else if (itt == it->second.location_block.end())
        return setResponseLine(itt, it, 
            404, "Not Found"
        );
    // 허용되지 않은 메소드
    else if (itt != it->second.location_block.end() && (!method.compare("GET") && !itt->second.methods[GET]) || (!method.compare("POST") && !itt->second.methods[POST])\
                || (!method.compare("DELETE") && !itt->second.methods[DELETE]))
        return setResponseLine(itt, it,
            405, "Method not allowed"
        );
    // HTTP 아닐 때
    else if (http_ver.substr(0, 4).compare("HTTP"))
        return setResponseLine(itt, it, 
            501, "Not Implemented"
        );
    // ver 1.1 아닐 때
    else if (http_ver.substr(5).compare("1.1"))
        return setResponseLine(itt, it, 
            505, "HTTP Version Not Supported"
        );
    // 요청 라인이 불량일 때
    else if (!temp.empty())
        return setResponseLine(itt, it, 
            400, "Bad Request"
        );
    // autoindex 인데 없는 디렉토리이거나 파일이 지워져서 없을 때
    else if ((itt->second.autoindex) && !opendir((it->second.root + itt->second.default_root.substr(1)).c_str()) || !ExistFile(itt->second.index_root))
        return setResponseLine(itt, it, 
            500, "Not Found"
        );
    // 리다이렉트
    else if (itt->second.ret)
        return setResponseLine(itt, it, 
            301, "Moved Permanently"
        );
    return setResponseLine(itt, it, 200, "OK");
}

void getDirectoryListingRecursive(const std::string& directory, std::vector<std::string>& listing) {
    DIR* dir = opendir(directory.c_str());
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_name[0] != '.') {  // 숨김 파일 제외
                std::string path = directory + "/" + entry->d_name;
                listing.push_back(path);
                if (entry->d_type == DT_DIR) {  // 디렉토리인 경우 재귀적으로 탐색
                    getDirectoryListingRecursive(path, listing);
                }
            }
        }
        closedir(dir);
    }
}

std::string Http::makeAutoindex(std::string root)
{
    struct dirent   *entityStruct;
    DIR             *directory;
    std::string     dirListPage;
    
    directory = opendir(root.c_str());
    dirListPage += "<html>\n";
    dirListPage += "<head>\n";
    dirListPage += "<title> Index of";
    dirListPage += root;
    dirListPage += "</title>\n";
    dirListPage += "</head>\n";
    dirListPage += "<body >\n";
    dirListPage += "<h1> Index of " + root + "</h1>\n";
    dirListPage += "<table style=\"width:80%; font-size: 15px\">\n";
    dirListPage += "<hr>\n";
    dirListPage += "<th style=\"text-align:left\"> File Name </th>\n";
    dirListPage += "<th style=\"text-align:left\"> Last Modification  </th>\n";
    dirListPage += "<th style=\"text-align:left\"> File Size </th>\n";

    struct stat file_stat;
    std::string file_path;

    while((entityStruct = readdir(directory)) != NULL)
    {
        if(strcmp(entityStruct->d_name, ".") == 0)
            continue;
        file_path = root + entityStruct->d_name;
        stat(file_path.c_str() , &file_stat);
        dirListPage += "<tr>\n";
        dirListPage += "<td>\n";
        dirListPage += "<a href=\"";
        dirListPage += entityStruct->d_name;
        if (S_ISDIR(file_stat.st_mode))
            dirListPage += "/";
        dirListPage += "\">";
        dirListPage += entityStruct->d_name;
        if (S_ISDIR(file_stat.st_mode))
            dirListPage += "/";
        dirListPage += "</a>\n";
        dirListPage += "</td>\n";
        dirListPage += "<td>\n";
        dirListPage += ctime(&file_stat.st_mtime);
        dirListPage += "</td>\n";
        dirListPage += "<td>\n";
        if (!S_ISDIR(file_stat.st_mode))
            dirListPage += ft_to_string(file_stat.st_size);
        dirListPage += "</td>\n";
        dirListPage += "</tr>\n";
    }
    dirListPage += "</table>\n";
    dirListPage += "<hr>\n";

    dirListPage += "</body>\n";
    dirListPage += "</html>\n";

    return dirListPage;
}

std::string Http::readFile(std::vector<std::pair<unsigned short, ServerBlock> >::iterator it, std::vector<std::pair<std::string, LocationBlock> >::iterator itt, std::string &msg) {
    std::string line, ret;
    std::ifstream file;
    std::vector<std::string>    listing;
    int code = 0;

    if (it == this->server_block.end())
        return makeHtml("<h1> 403에러났죠? ㅋㅋ <br>그렇게 하는거 아닌데 ㅋ</h1>\n");
    if (itt != it->second.location_block.end()) {
        if (msg.find("200", 5) != std::string::npos)
            code = 200;
        if (msg.find("404", 5) != std::string::npos || code) {
            if (code == 200 && itt->second.autoindex) {
                ret = makeAutoindex(it->second.root + itt->second.default_root.substr(1));
            } else {
                if (code == 200)
                    file.open(itt->second.index_root);
                else {
                    file.open(it->second.error_page);
                    if (!file.is_open())
                        ret = makeHtml("<h1> 404에러났죠? ㅋㅋ <br>그렇게 하는거 아닌데 ㅋ</h1>\n");
                }
                if (file.is_open())
                {
                    while (std::getline(file, line)) {
                        ret += line;
                        ret += "\n";
                    }
                }
                file.close();
            }
        }
    } else {
        ret = makeHtml("<h1>" + msg.substr(9, 3) + "에러났죠? ㅋㅋ <br>그렇게 하는거 아닌데 ㅋ</h1>\n");
    }
    return ret;
}

std::pair<std::string, std::string> Http::makeResponse(std::vector<std::pair<unsigned short, ServerBlock> >::iterator it, char *msg) {
    std::pair<std::string, std::string>   ret;
    std::string method, root, http_ver, temp;
    std::vector<std::pair<std::string, LocationBlock> >::iterator itt;
    std::stringstream ss, s;

    ss << msg;
    std::string tmp;
    std::getline(ss, tmp);
    s << tmp;
    s >> method >> root >> http_ver >> temp;
    if (root[root.length() - 1] == '/')
        root = root.substr(0, root.length() - 1);
    if (it != this->server_block.end())
        for (itt = it->second.location_block.begin(); itt != it->second.location_block.end() && root.compare(itt->second.default_root); itt++);
    ret.first = checkValidRequestLine(method, root, http_ver, temp, it, itt);
    ret.second = readFile(it, itt, ret.first);
    return (ret);
}*/