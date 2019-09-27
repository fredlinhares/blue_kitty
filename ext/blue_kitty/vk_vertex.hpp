// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_VK_VERTEX_H
#define BLUE_KITTY_VK_VERTEX_H 1

#include <glm/glm.hpp>

namespace BKVK
{
  typedef struct Vertex_t
  {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture_coord;
    glm::vec3 color;
  } Vertex;
}

#endif /* BLUE_KITTY_VK_VERTEX_H */
