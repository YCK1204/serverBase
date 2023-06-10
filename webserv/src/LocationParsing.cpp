#include "../header/Http.hpp"

LocationBlock Http::makeLocation(std::ifstream &file, std::stringstream &ss) {
    LocationBlock ret;
    std::string line, cmd, tmp, temp, tt;
    int types[6] = {}, t = 0;

    ss >> tmp >> temp >> tt;
    if (temp.compare("{")) {
        occurException(9, line, CONFIG, L_PARSING,
        "location is not started \"location default_root {\"");
    } else {
        if (!tt.empty()) {
            occurException(13, line, CONFIG, L_PARSING,
            "location is not started \"location default_root {\"");
        }
    }
    ret.default_root = tmp;
    for (int i = 0; i < 3; i++)
        ret.methods[i] = true;
    while (std::getline(file, line)) {
        if (line.empty())
            continue ;
        std::stringstream ss(line);
        ss >> cmd;
        if (!cmd.compare("}") && ++t)
            break ;
        if (!cmd.compare("method") && ++types[METHOD])
            buildLocationOption(ss, ret.methods);
        else if (!cmd.compare("autoindex") && ++types[AUTOINDEX])
            ret.autoindex = buildLocationOption(ss, "");
        else if (!cmd.compare("root") && ++types[L_ROOT])
            ret.root = buildLocationOption(ss);
        else if (!cmd.compare("index") && ++types[L_INDEX])
            ret.index = buildLocationOption(ss);
        else if (!cmd.compare("return") && ++types[RETURN])
            ret.redirect = buildLocationOption(ss);
        else if (!cmd.compare("cgi-bin") && ++types[CGI])
            ret.cgi = buildLocationOption(ss);
    }
    if (t != 1) {
        occurException(41, "", CONFIG, L_PARSING,
        "location is not finished \"}\"");
    }
    return ret;
}

void    Http::buildLocationOption(std::stringstream &ss, bool types[3]) {
    std::string t, t1, t2, t3;
    int type[3] = {}, tmp;

    for (int i = 0; i < 3; i++)
        types[i] = false;
    for (int i = 0; i < 4; i++) {
        ss >> t;
        if (t.empty())
            break ;
        tmp = distinctionMethods(t);
        type[tmp]++;
        types[tmp] = true;
    }
    if (!type[0] && !type[1] && !type[2]) {
        occurException(62, "method", CONFIG, L_PARSING,
        "empty method");
    }
    for (int i = 0; i < 3; i++) {
        if (type[i] > 1)
            occurException(67, "method", CONFIG, L_PARSING,
            "should noly appear once");
    }
}

int Http::distinctionMethods(std::string &method) {
    int ret;

    if (!method.compare("GET"))
        ret = 0;
    else if (!method.compare("POST"))
        ret = 1;
    else if (!method.compare("DELETE"))
        ret = 2;
    else {
        occurException(78, method, CONFIG, L_PARSING,
        "methods must (GET, POST, DELETE)");
    }
    return ret;
}

bool    Http::buildLocationOption(std::stringstream &ss, std::string tt) {
    std::string t;
    bool    ret;

    ss >> t >> tt;
    if (!t.compare("on"))
        ret = true;
    else if (!t.compare("off"))
        ret = false;
    else if (!tt.empty())
    {
        occurException(98, t, CONFIG, L_PARSING,
        "autoindex must on or off");
    }
    return ret;
}

std::string Http::buildLocationOption(std::stringstream &ss) {
    std::string t, tt;

    ss >> t >> tt;
    if (!tt.empty()) {
        occurException(110, tt, CONFIG, L_PARSING,
        "bad argument location option");
    }
    return t;
}