#pragma once
#include <GLFW/glfw3.h>
#include <cstdint>
#include <utility>
#include <vulkan/vulkan_core.h>
class VkTreang {
public:
  void run();

private:
  GLFWwindow *_window;
  VkInstance _instance;
  VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
  VkDevice _device;

  std::pair<uint32_t, const char **> initGLFW();
  void loop();

  void initInstance(std::pair<uint32_t, const char **> ext);
  void initPhysicalDevice();
  void initLogicalDevice();

  uint32_t findQueueFamilies(VkPhysicalDevice device);
};
