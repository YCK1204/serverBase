#include "../header/Http.hpp"

int main(int ac, char *av[])
{
	if (ac == 2)
	{
		try
		{
			std::cout << av[1] << std::endl;
			Http serv(static_cast<std::string>(av[1]));
			serv.printConfigInfo();

		}
		catch (std::exception &e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
    return 0;
}
