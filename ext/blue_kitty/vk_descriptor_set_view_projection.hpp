// SPDX-License-Identifier: MIT
#ifndef BLUE_KITTY_VK_DESCRIPTOR_SET_VIEW_PROJECTION_HPP
#define BLUE_KITTY_VK_DESCRIPTOR_SET_VIEW_PROJECTION_HPP 1

#include "loader.hpp"
#include "vk_descriptor_set_base.hpp"

namespace BKVK::DS // Descriptor set.
{
class ViewProjection: public Base
{
  friend class Loader::Stack<ViewProjection>;

  ViewProjection(const ViewProjection &vp) = delete;
  ViewProjection& operator=(const ViewProjection &vp) = delete;
  ViewProjection(const ViewProjection &&vp) = delete;
  ViewProjection& operator=(const ViewProjection &&vp) = delete;

 public:
  explicit ViewProjection(
      const std::shared_ptr<DSL::Base> &layout,
      const std::vector<std::shared_ptr<UniformBuffer>> &uniform_buffers);
  ~ViewProjection();

 private:
  Loader::Stack<ViewProjection> loader;

  void load_pool();
  void unload_pool();

  void load_buffers();
  void unload_buffers();
};
}

#endif /* BLUE_KITTY_VK_DESCRIPTOR_SET_VIEW_PROJECTION_HPP */
