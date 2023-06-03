#include "Http.hpp"

int main(int ac, char *av[])
{
    try
    {
        Http serv(static_cast<std::string>(av[1]));
        serv.printServerInfo(8002);
        
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}