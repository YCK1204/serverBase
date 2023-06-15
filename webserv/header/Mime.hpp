#ifndef MIME_HPP
# define MIME_HPP

#include <iostream>
#include <string>
#include <algorithm>
#include <map>

class Mime {
private:
    Mime(Mime const &m);
    Mime &operator = (Mime const &m);
    std::map<std::string, std::string>  type;
    std::map<int, std::string>          status;
public:
    Mime();
    ~Mime();
    std::string getStatus(const int &n);
    std::string getType(const std::string &type);
};

#endif