// SPDX-License-Identifier: MIT
#include "loader.hpp"

namespace Loader
{
  Error::Error()
  {
  }

  Error::Error(const std::string &m):
    message{m}
  {
  }

  Error::Error(const char &m):
    message{m}
  {
  }
}
