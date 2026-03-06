#include <algorithm>
#include <cinttypes>
#include <limits>
#include <set>
#include <string>
#include <sys/types.h>
#define GLFW_INCLUDE_VULKAN
#include "fmt/base.h"
#include "vkTreang.h"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

void VkTreang::run() {
  std::pair<uint32_t, const char **> ext = initGLFW();
  initInstance(ext);
  initPhysicalDevice();
  initPhysicalDevice();
  initSurface();
  loop();
}

std::pair<uint32_t, const char **> VkTreang::initGLFW() {

  if (!glfwInit()) {
    throw std::runtime_error("GLFW init failed");
  }

  if (!glfwVulkanSupported()) {
    throw std::runtime_error("Vulkan unsopported");
  }

  uint32_t extCount = 0;
  const char **ext = glfwGetRequiredInstanceExtensions(&extCount);

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  GLFWwindow *window = glfwCreateWindow(640, 480, "Window", NULL, NULL);
  _window = window;

  glfwShowWindow(_window);

  return std::pair<uint32_t, const char **>(extCount, ext);
};
void VkTreang::loop() {
  while (!glfwWindowShouldClose(_window)) {
    glfwPollEvents();
  }
}

void VkTreang::initInstance(std::pair<uint32_t, const char **> ext) {
  VkApplicationInfo appInfo{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = "vkTreang",
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "No engine",
      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
      .apiVersion = VK_API_VERSION_1_0,
  };

  VkInstanceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &appInfo,
      .ppEnabledLayerNames = 0,
      .enabledExtensionCount = ext.first,
      .ppEnabledExtensionNames = ext.second,
  };

  if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create instance.");
  }
};

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

void VkTreang::initPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

  for (VkPhysicalDevice device : devices) {
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(device, &props);
    fmt::println("Supported gpu: {}", props.deviceName);
  }

  _physicalDevice = devices[0];
};

void VkTreang::initLogicalDevice() {
  float queuePriority = 1.0f;
  VkDeviceQueueCreateInfo queueCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .queueFamilyIndex = findQueueFamilies(),
      .queueCount = 1,
      .pQueuePriorities = &queuePriority,
  };

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &queueCreateInfo,
      .enabledExtensionCount = static_cast<uint32_t>(_deviceExtensions.size()),
      .ppEnabledExtensionNames = _deviceExtensions.data(),
      .pEnabledFeatures = &deviceFeatures,
  };

  if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }
}

void VkTreang::initSurface() {
  if (glfwCreateWindowSurface(_instance, _window, NULL, &_surface) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to create surface");
  }
};

void VkTreang::createSwapChain() {
  SwapChainSupportDetails details = querySwapChainSupport();

  VkExtent2D extend = chooseSwapExtent(details.capabilities);
  VkSurfaceFormatKHR format = chooseSwapSurfaceFormat(details.formats);
  VkPresentModeKHR presentMode = chooseSwapPresentMode(details.presentModes);

  _swapChainImageFormat = format.format;
  _swapChainExtent = extend;

  uint32_t imageCount = details.capabilities.minImageCount + 1;

  if (details.capabilities.maxImageCount > 0 &&
      imageCount > details.capabilities.maxImageCount) {
    imageCount = details.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR swapChainInfo{
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface = _surface,
      .minImageCount = imageCount,
      .imageFormat = format.format,
      .imageColorSpace = format.colorSpace,
      .imageExtent = extend,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      .preTransform = details.capabilities.currentTransform,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = presentMode,
      .clipped = VK_TRUE,
      .oldSwapchain = VK_NULL_HANDLE,
  };

  if (vkCreateSwapchainKHR(_device, &swapChainInfo, nullptr, &_swapChain) !=
      VK_SUCCESS) {
    throw std::runtime_error("Faild to create swapchain");
  }

  uint32_t imagesCount = 0;
  vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr);
  _swapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount,
                          _swapChainImages.data());
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
