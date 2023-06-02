#include "Http.hpp"

int main(int ac, char *av[])
{
    Http    serv;

    try
    {
        serv.ParsingConfig(static_cast<std::string>(av[1]));
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}