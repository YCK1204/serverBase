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
    return spaceTrim(ret);
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
    std::string     ret, req_msg = clients[clnt_sock].request;
    ServerBlock     server = getServer(ft_to_string(clients[clnt_sock].port), clients[clnt_sock].root);
    LocationBlock   location = getLocation(clients[clnt_sock].root, server);

    ret += "HTTP/1.1 " + ft_to_string(status) + mime.getStatus(status) + "\r\n";
    ret += "Connection: " + clients[clnt_sock].connection + "\r\n";
    ret += "Content-length: " + ft_to_string(length) + "\r\n";
    ret += "Content-type: " + mime.getType(clients[clnt_sock].file_extension) + "\r\n";
    if (location.ret)
        ret += "Location: " + location.redirect + "\r\n";
    ret += getDate() + "\r\n";
    if (!clients[clnt_sock].connection.compare("keep-alive"))
        ret += "keep-alive: timeout=" + ft_to_string(OUTTIME) + ", max=" + ft_to_string(LISTEN_SIZE) + "\r\n";
    ret += "Server: " + server.server_name + "\r\n\r\n";
    return ret;
}



std::string Http::getContent(int clnt_sock) {
    ServerBlock     server      = getServer(ft_to_string(clients[clnt_sock].port), clients[clnt_sock].root);
    LocationBlock   location    = getLocation(clients[clnt_sock].root, server);
    std::string     ret, file_root, line;
    std::ifstream   file;
    std::size_t     len;

    clients[clnt_sock].file_extension = "html";
    if (location.autoindex) {
        status = 200;
        return buildAutoindex(server.root + location.default_root.substr(1));
    }
    if (location.ret) {
        status = 301;
        return "";
    }
    file_root = getIndexRoot(server, location);
    file.open(file_root.c_str());
    if (file.is_open()) {
        while (std::getline(file, line)) {
            ret += line;
            line.clear();
        }
        status = 200;
        len = file_root.rfind(".");
        if (len != std::string::npos)
            clients[clnt_sock].file_extension = file_root.substr(len + 1);
        else
            clients[clnt_sock].file_extension = "";
    }
    else
        err = 500;
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

std::pair<std::string, std::string> Http::getResponse(int clnt_sock) {
    std::string req_msg = clients[clnt_sock].request;
    std::pair<std::string, std::string> ret;

    getData(clnt_sock);
    checkRequestMsg(clnt_sock);

    if (err) {
        ret.first   = buildErrorMsg(clnt_sock);
        ret.second  = buildErrorHtml(err);
        ret.first += "Content-length: " + ft_to_string(ret.second.length()) + "\r\n\r\n";
    }
    else {
        ret.second  = getContent(clnt_sock);
        if (err)
            ret.first = buildErrorMsg(clnt_sock);
        else {
            ret.first   = getMsg(clnt_sock, ret.second.length());
        }
    }
    return ret;
}

void    Http::checkRequestMsg(int clnt_sock) {
    std::string         method      = clients[clnt_sock].method;
    std::string         root        = clients[clnt_sock].root;
    std::string         connection  = clients[clnt_sock].connection;
    int                 port        = clients[clnt_sock].port;
    ServerBlock         server      = getServer(ft_to_string(port), root);
    LocationBlock       location    = getLocation(root, server);
    bool                methods[3]  = {location.methods[0], location.methods[1], location.methods[2]};

    if (method.compare("GET") && method.compare("POST") && method.compare("DELETE"))
        err = 400;
    else if (!err && !methods[GET] && !method.compare("GET") && !methods[POST] && !method.compare("POST") && !methods[DELETE] && !method.compare("DELETE"))
        err = 405;
    else if (!err){
        if (root.length() > MAX_ROOT_LEN)
            err = 414;
        else if (location.default_root.compare(root))
            err = 404;
        else if (clients[clnt_sock].http.compare("HTTP"))
            err = 501;
        else if (clients[clnt_sock].http_ver.compare("1.1"))
            err = 505;
        else if (checkValidateAddress(clients[clnt_sock].addr))
            err = 403;
        else if (port == 65536 || server.port != port)
            err = 421;
        else if (connection.compare("keep-alive") && connection.compare("close"))
            err = 400;
    }
}