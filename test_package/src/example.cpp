#include "efano/efano.hpp"

#include <iostream>

int main()
{
    if (efano::entry_point()) {
        std::cout << "Test package is working!" << std::endl;
    }
    return 0;
}
