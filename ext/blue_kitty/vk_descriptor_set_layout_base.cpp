// SPDX-License-Identifier: MIT
#include "vk_descriptor_set_layout_base.hpp"

namespace BKVK::DSL // Descriptor set layout.
{
Base::Base(const std::shared_ptr<Device> &device):
    device{device}
{
}
}
