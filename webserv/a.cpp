#include <iostream>
#include <iterator>
#include <string>

std::string    spaceTrim(std::string str) {
    int start, end;
    for (start = 0; str[start]; start++) {
        if (str[start] != ' ')
            break ;
    }
    std::cout << str.length() << std::endl;
    for (end = str.length() - 1; end > 0; end--) {
        if (str[end] != ' ')
            break ;
    }
    return str.substr(start, end - start + 1);
}

int main() {
    std::string tmp;

    tmp = "hello         !";
    std::cout << spaceTrim(tmp) << "sss" << std::endl;

}