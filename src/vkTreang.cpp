#include "vkTreang.h"
#include "fmt/base.h"
#include <GLFW/glfw3.h>
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

uint32_t VkTreang::findQueueFamilies(VkPhysicalDevice device) {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
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
      .queueFamilyIndex = findQueueFamilies(_physicalDevice),
      .queueCount = 1,
      .pQueuePriorities = &queuePriority,
  };

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &queueCreateInfo,
      .pEnabledFeatures = &deviceFeatures,
  };

  if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }
}
