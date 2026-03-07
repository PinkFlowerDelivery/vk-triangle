#include "vkTreang.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <algorithm>
#include <limits>
#include <set>
#include <string>
#include <vulkan/vulkan_core.h>

uint32_t VkTreang::findQueueFamilies() {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount,
                                           nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

  vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount,
                                           queueFamilies.data());

  int32_t i = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      return i;
    }

    i++;
  }

  return i;
};

bool VkTreang::checkDeviceExtensionSupport() {
  uint32_t extCount = 0;
  vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &extCount,
                                       nullptr);

  std::vector<VkExtensionProperties> extProps{extCount};
  vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &extCount,
                                       extProps.data());

  std::set<std::string> requiredExt(_deviceExtensions.begin(),
                                    _deviceExtensions.end());

  for (const auto &extProp : extProps) {
    requiredExt.erase(extProp.extensionName);
  }

  return requiredExt.empty();
};

SwapChainSupportDetails VkTreang::querySwapChainSupport() {
  SwapChainSupportDetails details;

  // capabilities
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice, _surface,
                                            &details.capabilities);

  // formats
  uint32_t formatCount = 0;

  vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &formatCount,
                                       nullptr);

  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface,
                                         &formatCount, details.formats.data());
  }

  // presentModes

  uint32_t presentModeCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface,
                                            &presentModeCount, nullptr);

  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface,
                                              &presentModeCount,
                                              details.presentModes.data());
  }

  return details;
};

VkPresentModeKHR VkTreang::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes) {

  for (const auto &availablePresentModes : availablePresentModes) {
    if (availablePresentModes == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availablePresentModes;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
};

VkSurfaceFormatKHR VkTreang::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats) {
  for (const auto &availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  return availableFormats[0];
};

VkExtent2D
VkTreang::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    int32_t w, h;
    glfwGetFramebufferSize(_window, &w, &h);

    VkExtent2D actualExtend = {static_cast<uint32_t>(w),
                               static_cast<uint32_t>(h)};

    actualExtend.width =
        std::clamp(actualExtend.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
    actualExtend.width =
        std::clamp(actualExtend.width, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);

    return actualExtend;
  }
};
