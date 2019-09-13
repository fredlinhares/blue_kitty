// SPDX-License-Identifier: MIT

#ifndef BLUE_KITTY_VK_DESCRIPTOR_SET_LAYOUT_VIEW_PROJECTION_HPP
#define BLUE_KITTY_VK_DESCRIPTOR_SET_LAYOUT_VIEW_PROJECTION_HPP 1

#include "vk_descriptor_set_layout_base.hpp"

namespace BKVK::DSL // Descriptor set layout.
{
class ViewProjection: public Base
{
  ViewProjection(const ViewProjection &vp) = delete;
  ViewProjection& operator=(const ViewProjection &vp) = delete;
  ViewProjection(const ViewProjection &&vp) = delete;
  ViewProjection& operator=(const ViewProjection &&vp) = delete;

 public:
  explicit ViewProjection(const std::shared_ptr<Device> &device);
  ~ViewProjection();

};
}

#endif /* BLUE_KITTY_VK_DESCRIPTOR_SET_LAYOUT_VIEW_PROJECTION_HPP */
