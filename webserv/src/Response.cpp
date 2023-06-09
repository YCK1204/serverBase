#include "../header/Http.hpp"
/* line */
std::string Http::getMethod(std::string req_msg) {
    std::stringstream ss(req_msg);
    std::string ret;

    ss >> ret;
    return spaceTrim(ret);
}

std::string Http::getRoot(std::string req_msg) {
    std::stringstream   ss(req_msg);
    std::string         tmp, ret;

    ss >> tmp >> ret;
    ret = spaceTrim(ret);
    if (ret.length() > 1 && ret[ret.length() - 1] == '/')
        ret = ret.substr(0, ret.length() - 1);
    return ret;
}

std::string Http::getHTTP(std::string req_msg) {
    std::stringstream   ss(req_msg);
    std::string         t, tt, ret;

    ss >> t >> tt >> ret;
    if (ret.find("HTTP") != std::string::npos)
        ret = ret.substr(0, 4);
    return spaceTrim(ret);
}

std::string Http::getHttpVer(std::string req_msg) {
    std::stringstream   ss(req_msg);
    std::string         t, tt, ret;
    size_t              len = 0;

    ss >> t >> tt >> ret;
    if ((len = ret.find("/")) != std::string::npos)
        ret = ret.substr(len + 1);
    return spaceTrim(ret);
}
/* line */

/* header */
std::string Http::getAddress(std::string req_msg) {
    std::stringstream   ss(req_msg);
    std::string         line, ret, cmp;
    size_t              len;

    cmp = "Host:";
    while (std::getline(ss, line)) {
        if (line.find(cmp) != std::string::npos) {
            break ;
        }
        line.clear();
    }
    len = line.rfind(":");
    if (len == std::string::npos)
        return "11.11.11.11";
    ret = line.substr(5, len);
    if (!ret.compare("localhost"))
        ret = "0.0.0.0";
    return spaceTrim(ret);
}

std::string Http::getPort(std::string req_msg) {
    std::stringstream   ss(req_msg);
    std::string         line, ret, cmp;
    size_t              len;

    cmp = "Host:";
    while (std::getline(ss, line)) {
        if (line.find(cmp) != std::string::npos)
            break ;
        line.clear();
    }
    if (std::strncmp(line.c_str(), cmp.c_str(), cmp.length()))
        return (ret = "65536");
    len = line.rfind(":");
    if (len == std::string::npos)
        return (ret = "65536");
    ret = line.substr(len + 1);
    return spaceTrim(ret);
}

std::string Http::getConnection(std::string req_msg) {
    std::stringstream   ss(req_msg);
    std::string         line, ret, cmp;
    size_t              len;

    cmp = "Connection:";
    while (std::getline(ss, line)) {
        if (line.find(cmp))
            break ;
        line.clear();
    }
    if (line.find(cmp) == std::string::npos)
        return "keep-alive";
    if (!std::strncmp(line.c_str(), cmp.c_str(), cmp.length()) && (len = line.rfind(":")) != std::string::npos)
        ret = line.substr(len + 1);
    return spaceTrim(ret);
}
/* header */

std::string Http::getMsg(int clnt_sock, int length) {
    std::string     ret, file_path, req_msg = clients[clnt_sock].request;
    ServerBlock     server = getServer(ft_to_string(clients[clnt_sock].port), clients[clnt_sock].root);
    LocationBlock   location = getLocation(clients[clnt_sock].root, server);
    struct stat file_stat;

    ret += "HTTP/1.1 " + ft_to_string(status) + mime.getStatus(status) + "\r\n";
    ret += "Connection: " + clients[clnt_sock].connection + "\r\n";
    ret += "Content-length: " + ft_to_string(length) + "\r\n";
    ret += "Content-type: " + mime.getType(clients[clnt_sock].file_extension) + "\r\n";
    if (location.ret) {
        ret += "Location: " + location.redirect + "\r\n";
        status = 301;
    }
    ret += getDate() + "\r\n";
    if (!clients[clnt_sock].connection.compare("keep-alive"))
        ret += "keep-alive: timeout=" + ft_to_string(TIMEOUT) + ", max=" + ft_to_string(REQUEST_CNT) + "\r\n";
    file_path = getIndexRoot(server, location);
    if (stat(file_path.c_str(), &file_stat) >= 0)
        ret += "Last-Modified: " + formatTime(file_stat.st_mtime) + "\r\n";
    ret += "Server: " + server.server_name + "\r\n\r\n";
    return ret;
}

std::string Http::getContent(int clnt_sock) {
    std::string     ret, root, line;
    std::ifstream   file;
    std::vector<ServerBlock>::iterator it;
    std::vector<LocationBlock>::iterator itt;
    bool    f;
    
    for (it = this->server.begin(); it != this->server.end(); it++) {
        f = false;
        if (it->port == clients[clnt_sock].port) {
            for (itt = it->location.begin(); itt != it->location.end(); itt++) {
                if (!std::strncmp(itt->default_root.c_str(), clients[clnt_sock].root.c_str(), itt->default_root.length())) {
                    if (itt->autoindex) {
                        f = true;
                        break ;
                    }
                }
            }
        }
        if (f)
            break ;
    }
    clients[clnt_sock].file_extension = "html";
    if (f) {
        root = it->root;
        root += clients[clnt_sock].root;
        if (isDir(root)) {
            ret = buildAutoindex(it->root, clients[clnt_sock].root);
        } else {
            if (isFile(root)) {
                file.open(root.c_str());
                while (std::getline(file, line)) {
                    ret += line + "\n";
                    line.clear();
                }
                file.close();
            }
            else
                err = 500;
        }
    } else {
        ServerBlock server = getServer(ft_to_string(clients[clnt_sock].port), clients[clnt_sock].root);
        LocationBlock location = getLocation(clients[clnt_sock].root, server);
        if (!clients[clnt_sock].root.compare(location.default_root)) {
            if (location.ret)
                return "";
            root = getIndexRoot(server, location);
            file.open(root.c_str());
            if (file.is_open()) {
                while (std::getline(file, line))
                    ret += line + "\n";
                size_t len = root.rfind(".");
                if (len != std::string::npos)
                    clients[clnt_sock].file_extension = root.substr(len + 1);
            }
            else
                err = 500;
        }
        else
            err = 404;
    }
    if (err)
        ret = buildErrorHtml(err);
    return ret;
}

void    Http::getData(int clnt_sock) {
    std::string req_msg = clients[clnt_sock].request, addr;
    int         cnt = 0;
    size_t      tmp = 0, len;

    addr = getAddress(req_msg);
    clients[clnt_sock].root         = getRoot(req_msg);
    clients[clnt_sock].http         = getHTTP(req_msg);
    clients[clnt_sock].method       = getMethod(req_msg);
    clients[clnt_sock].http_ver     = getHttpVer(req_msg);
    clients[clnt_sock].connection   = getConnection(req_msg);
    clients[clnt_sock].port         = ft_stoi(getPort(req_msg));
    for (; (len = addr.find(".", tmp)) != std::string::npos;) {
        cnt++;
        if (cnt > 3) {
            clients[clnt_sock].addr[cnt - 1] = 4242;
            break ;
        }
        clients[clnt_sock].addr[cnt - 1] = ft_stoi(addr.substr(tmp, len - tmp));
        tmp = len + 1;
    }
    if (cnt == 3)
        clients[clnt_sock].addr[cnt] = ft_stoi(addr.substr(tmp, len - tmp));
    else
        clients[clnt_sock].addr[cnt] = 4242;
}

std::pair<std::string, std::string> Http::getDeleteReseponse(int clnt_sock) {
    std::string body = clients[clnt_sock].body_request;
    std::pair<std::string, std::string> ret;

    return ret;
}

std::string Http::failedSignUp(std::string id, std::string passwd) {
    std::string ret, msg;

    msg += "<form method=\"post\">\n";
    msg += "<input type=\"hidden\" name=\"_method\" value=\"post\"/>\n";
    msg += "<label for=\"name\">닉네임 :</label>\n";
    msg += "<input type=\"text\" name=\"id\" id=\"id\" value=\"" + id + "\">\n";
    msg += "<label for=\"password\">비밀번호 :</label>\n";
    msg += "<input type=\"password\" name=\"password\" id=\"password\" value=\"" + passwd + "\">\n";
    msg += "<input type=\"submit\" value=\"Submit\">\n";
    msg += "</form>\n";

    ret = buildHtml(msg);
    return ret;
}

bool    Http::isValidPasswd(std::string passwd) {
    
}

std::pair<std::string, std::string> Http::getPostReseponse(int clnt_sock) {
    std::string body = clients[clnt_sock].body_request;
    std::string method, id, passwd;
    std::pair<std::string, std::string> ret;

    std::size_t len = body.find("&");
    method = body.substr(8, len);
    if (!method.compare("POST")) {
        id = body.substr(len + 3, body.find("&", len + 1));
        passwd = body.substr(body.rfind("=") + 1);
        if (member_info.find(id) != member_info.end()) {
            ret.second = failedSignUp(id, passwd);
            ret.first = getMsg(clnt_sock, ret.second.length());
        } else {
            if (isValidPasswd(passwd)) {

            } else {

            }
        }
    }
    else
        ret = getDeleteReseponse(clnt_sock);
    return ret;
}

std::pair<std::string, std::string> Http::getResponse(int clnt_sock) {
    std::pair<std::string, std::string> ret;

    getData(clnt_sock);
    checkRequestMsg(clnt_sock);
    if (err) {
        ret.first   = buildErrorMsg();
        ret.second  = buildErrorHtml(err);
    } else {
        if (!clients[clnt_sock].method.compare("POST"))
            ret = getPostReseponse(clnt_sock);
        else {
            ret.second = getContent(clnt_sock);
            if (err)
                ret.first = buildErrorMsg();
            else
                ret.first = getMsg(clnt_sock, ret.second.length());
        }
    }
    return ret;
}

bool    Http::isValidAddress(ServerBlock server, int addr[4]) {
    if (!server.host.compare("0.0.0.0"))
        return true;
    std::string tmp;
    for (int i = 0; i < 3; i++) {
        tmp += ft_to_string(addr[i]) + ".";
    }
    tmp += ft_to_string(addr[3]);
    return !server.host.compare(tmp);
}

void    Http::checkRequestMsg(int clnt_sock) {
    std::string         method      = clients[clnt_sock].method;
    std::string         root        = clients[clnt_sock].root;
    std::string         connection  = clients[clnt_sock].connection;
    int                 port        = clients[clnt_sock].port;
    ServerBlock         server      = getServer(ft_to_string(port), root);
    LocationBlock       location    = getLocation(root, server);
    bool                methods[3]  = {location.methods[0], location.methods[1], location.methods[2]};

    if (err)
        return ;
    if (method.compare("GET") && method.compare("POST") && method.compare("DELETE"))
        err = 400;
    else if (!err && !methods[GET] && !method.compare("GET") && !methods[POST] && !method.compare("POST") && !methods[DELETE] && !method.compare("DELETE"))
        err = 405;
    if (!err) {
        if (root.length() > MAX_ROOT_LEN)
            err = 414;
        else if (clients[clnt_sock].http.compare("HTTP"))
            err = 501;
        else if (clients[clnt_sock].http_ver.compare("1.1"))
            err = 505;
        else if (!isValidAddress(server, clients[clnt_sock].addr))
            err = 403;
        else if (port == 65536 || server.port != port)
            err = 421;
        else if (connection.compare("keep-alive") && connection.compare("close"))
            err = 400;
    }
}