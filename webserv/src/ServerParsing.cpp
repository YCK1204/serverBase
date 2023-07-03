#include "../header/Http.hpp"

ServerBlock Http::makeServer(std::ifstream &file) {
	ServerBlock ret;
	std::string line, cmd;
    int types[7] = {}, t = 0;

	while (std::getline(file, line)) {
		if (line.empty())
			continue ;
		std::stringstream ss(line);
		ss >> cmd;
        if (!cmd.compare("}") && ++t)
            break ;
		if (cmd.compare("location"))
			setServerOption(ss, cmd, ret, types);
        else 
            ret.location.push_back(makeLocation(file, ss));
    }
    checkValidServerOption(types);
    if (t != 1) {
        occurException(21, "", CONFIG, S_PARSING,
        "server is not finished \"}\"");
    }
	return ret;
}

std::string Http::buildServerOption(std::stringstream &ss, s_block_type type) {
    std::string ret, tmp, obj;

    switch (type) {
        case HOST        : obj = "host"              ; break ;
        case S_ROOT      : obj = "root"              ; break ;
        case LISTEN      : obj = "listen"            ; break ;
        case S_INDEX     : obj = "index"             ; break ;
        case BODY_SIZE   : obj = "client_body_size"  ; break ;
        case ERROR_PAGE  : obj = "error_page"        ; break ;
        case SERVER_NAME : obj = "server_name"       ; break ;
    }
    ss >> ret >> tmp;
    if (!tmp.empty()) {
        occurException(28, tmp, CONFIG, S_PARSING,
        "server option error");
    }
    return ret;
}

void    Http::setServerOption(std::stringstream &ss, std::string cmd, ServerBlock &ret, int types[7]) {
    if (!cmd.compare("listen") && ++types[LISTEN])
        ret.port = ft_stoi(buildServerOption(ss, LISTEN).c_str(), HOST);
    else if (!cmd.compare("error_page") && ++types[ERROR_PAGE])
        ret.error_page = buildServerOption(ss, ERROR_PAGE);
    else if (!cmd.compare("host") && ++types[HOST])
        ret.host = buildServerOption(ss, HOST);
    else if (!cmd.compare("client_body_size") && ++types[BODY_SIZE])
        ret.client_body_size = ft_stoi(buildServerOption(ss, BODY_SIZE), BODY_SIZE);
    else if (!cmd.compare("index") && ++types[S_INDEX])
        ret.index = buildServerOption(ss, S_INDEX);
    else if (!cmd.compare("root") && ++types[S_ROOT])
        ret.root = buildServerOption(ss, S_ROOT);
    else if (!cmd.compare("server_name") && ++types[SERVER_NAME])
        ret.server_name = buildServerOption(ss, SERVER_NAME);
    else {
        occurException(48, cmd, CONFIG, S_PARSING,
        "server option error");
    }
}

void    Http::checkValidServerOption(int types[7]) {
    std::string msg, reason;

    if (types[LISTEN] != 1) {
        msg = "listen";
        reason = "should only appear once";
    }
    else if (types[ERROR_PAGE] != 1) {
        msg = "error_page";
        reason = "should only appear once";
    }
    else if (types[HOST] != 1) {
        msg = "host";
        reason = "should only appear once";
    }
    else if (types[BODY_SIZE] != 1) {
        msg = "client body size";
        reason = "should only appear once";
    }
    else if (types[S_INDEX] > 1) {
        msg = "client body size";
        reason = "must appear at most once";
    }
    else if (types[S_ROOT] > 1) {
        msg = "server root";
        reason = "must appear at most once";
    }
    else if (types[SERVER_NAME] > 1) {
        msg = "server name";
        reason = "must appear at most once";
    }
    if (!msg.empty()) {
        occurException(69, msg, CONFIG, S_PARSING, reason);
    }
}