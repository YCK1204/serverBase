#include <iostream>
#include <string>

void    recursive_to_string(int n, std::string &ret){
    if (n >= 10) {
		recursive_to_string(n / 10, ret);
		ret += (n % 10) + '0';
	}
    else
	    ret += n + '0';
}

std::string ft_to_string(int n, std::string ret) {
    int neg = 0;
    int minimum = 0;
    if (n == -2147483648) {
        minimum = 1;
        n++;
    }
    if (n < 0)
        ret += '-';
    recursive_to_string(n, ret);
    if
    return ret;
}

int main(){
    std::cout << ft_to_string(-2147483647, "") << std::endl;
}