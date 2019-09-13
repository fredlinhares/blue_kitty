// SPDX-License-Identifier: MIT
#include "log.hpp"

#include <iostream>

namespace BKGE
{

// For now, log will just print to command line. This layer of abstraction make
// it easy to move the output to a file in the future.
namespace Log
{
void header(const std::string &str)
{
  std::cout << "\n\e[1;34m" << str << "\e[0;0m\n";
}

void standard(const std::string &str)
{
  std::cout << str << std::endl;
}

}
}
