#pragma once
#include <GLFW/glfw3.h>
#include <cstdint>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

class VkTreang {
public:
  void run();

private:
  GLFWwindow *_window;
  VkInstance _instance;
  VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
  VkDevice _device;
  VkSurfaceKHR _surface;
  VkSwapchainKHR _swapChain;
  std::vector<VkImage> _swapChainImages;
  VkFormat _swapChainImageFormat;
  VkExtent2D _swapChainExtent;

  const std::vector<const char *> _deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  std::pair<uint32_t, const char **> initGLFW();
  void loop();

  void initInstance(std::pair<uint32_t, const char **> ext);
  void initPhysicalDevice();
  void initLogicalDevice();
  void initSurface();
  void createSwapChain();

  uint32_t findQueueFamilies();
  bool checkDeviceExtensionSupport();

  SwapChainSupportDetails querySwapChainSupport();
  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
};
