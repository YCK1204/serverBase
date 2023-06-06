#include "../header/Http.hpp"

int main(int ac, char *av[]) {
	int	ret = 0;
	
	if (ac == 2) {
		try {
			Http serv(static_cast<std::string>(av[1]));
		} catch (std::exception &e) {
			ret = 1;
			std::cerr << e.what() << std::endl;
		}
	}
	else {
		std::cerr << "Error : No Argument" << std::endl;
		ret = 1;
	}
    return ret;
}
