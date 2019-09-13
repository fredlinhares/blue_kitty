// SPDX-License-Identifier: MIT
#include "vk_swapchain.hpp"

#include <sstream>

#include "log.hpp"

namespace BKVK
{
Swapchain::Swapchain(const std::shared_ptr<BKVK::Device> &device):
    loader{this},
    device{device}
{
  this->loader.add(&Swapchain::load_swapchain, &Swapchain::unload_swapchain);
  this->loader.add(&Swapchain::load_image_views,
                   &Swapchain::unload_image_views);

  try
  {
    this->loader.load();
  }
  catch(Loader::Error le)
  {
    std::string base_error{"Could not initialize Vulkan Swapchain → "};
    base_error += le.message;
    throw Loader::Error{base_error};
  }
}

Swapchain::~Swapchain()
{
  this->loader.unload();
}

void Swapchain::load_swapchain()
{
  /*
    uint32_t vk_display_properties_count;
    std::vector<VkDisplayPropertiesKHR> vk_display_properties;
  */

  // Physical device surface capabilities.
  VkSurfaceCapabilitiesKHR surface_capabilities = {};
  if(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
         this->device->get_vk_physical_device(),
         this->device->get_instance()->get_surface(),
         &surface_capabilities) != VK_SUCCESS)
    throw Loader::Error{"Vulkan failed to get display surface capabilities."};

  if(this->device->get_instance()->get_core_data()->debug)
  {
    std::ostringstream txt;

    BKGE::Log::header("Physical devices surface capabilities.");
    txt << "Minimum image count: " << surface_capabilities.minImageCount
        << std::endl;
    txt << "Maximum image count: " << surface_capabilities.maxImageCount
        << std::endl;
    // txt << ": " << surface_capabilities.currentExtent;
    txt << "Minimum image extent: " << "width: " <<
        surface_capabilities.minImageExtent.width << ", height: " <<
        surface_capabilities.minImageExtent.height << std::endl;
    txt << "Maximum image extent: " << "width: " <<
        surface_capabilities.maxImageExtent.width << ", height: " <<
        surface_capabilities.maxImageExtent.height<< std::endl;
    txt << "Maximum image array layers: " <<
        surface_capabilities.maxImageArrayLayers << std::endl;
    /*txt << "Supported transforms: " <<
      surface_capabilities.supportedTransforms;
      txt << "Current transform: " <<
      surface_capabilities.currentTransform;
      txt << "Supported composite alpha: " <<
      surface_capabilities.supportedCompositeAlpha;
      txt << "Supported usage flags: " <<
      surface_capabilities.supportedUsageFlags;*/

    BKGE::Log::standard(txt.str());
  }

  // Surface formats.
  uint32_t vk_surface_format_count;
  std::vector<VkSurfaceFormatKHR> vk_surface_formats;
  vkGetPhysicalDeviceSurfaceFormatsKHR(
      this->device->get_vk_physical_device(),
      this->device->get_instance()->get_surface(),
      &vk_surface_format_count, nullptr);
  vk_surface_formats.resize(vk_surface_format_count);
  vkGetPhysicalDeviceSurfaceFormatsKHR(
      this->device->get_vk_physical_device(),
      this->device->get_instance()->get_surface(),
      &vk_surface_format_count, vk_surface_formats.data());

  VkSwapchainCreateInfoKHR swapchain_create_info = {};
  swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchain_create_info.pNext = nullptr;
  swapchain_create_info.flags = 0;
  swapchain_create_info.surface = this->device->get_instance()->get_surface();
  swapchain_create_info.minImageCount = 3; // triple buffering.

  this->vk_image_format = vk_surface_formats[0].format;
  swapchain_create_info.imageFormat = this->vk_image_format;
  swapchain_create_info.imageColorSpace = vk_surface_formats[0].colorSpace;

  swapchain_create_info.imageExtent = {
    this->device->get_instance()->get_core_data()->screen_width,
    this->device->get_instance()->get_core_data()->screen_height};
  swapchain_create_info.imageArrayLayers = 1;
  swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapchain_create_info.queueFamilyIndexCount = 0;
  swapchain_create_info.pQueueFamilyIndices = nullptr;
  swapchain_create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchain_create_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
  swapchain_create_info.clipped = VK_FALSE;
  swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

  if(vkCreateSwapchainKHR(this->device->get_vk_device(), &swapchain_create_info,
                          nullptr, &this->vk_swapchain) != VK_SUCCESS)
    throw Loader::Error{"Vulkan failed to create swapchain."};

  uint32_t swap_images_count;
  vkGetSwapchainImagesKHR(this->device->get_vk_device(), this->vk_swapchain,
                          &swap_images_count, nullptr);
  vk_images.resize(swap_images_count);
  vkGetSwapchainImagesKHR(this->device->get_vk_device(), this->vk_swapchain,
                          &swap_images_count, vk_images.data());
}

void Swapchain::unload_swapchain()
{
  vkDestroySwapchainKHR(this->device->get_vk_device(), this->vk_swapchain,
                        nullptr);
}

void Swapchain::load_image_views()
{
  this->vk_image_views.resize(this->vk_images.size());
  for (std::vector<VkImageView>::size_type i = 0;
       i < this->vk_image_views.size(); i++)
  {
    VkImageViewCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = vk_images[i];
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format = vk_image_format;
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;

    if(vkCreateImageView(this->device->get_vk_device(), &create_info, nullptr,
                         &this->vk_image_views[i]))
      throw Loader::Error{"Could no create Image View for swapchain."};
  }
}

void Swapchain::unload_image_views()
{
  for(auto vk_img_view: this->vk_image_views)
    vkDestroyImageView(this->device->get_vk_device(), vk_img_view, nullptr);
}

}
