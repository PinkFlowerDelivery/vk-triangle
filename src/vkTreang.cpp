#define GLFW_INCLUDE_VULKAN

#include "vkTreang.h"
#include "fmt/base.h"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <stdexcept>
#include <sys/types.h>
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
