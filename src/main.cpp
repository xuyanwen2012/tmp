#include <cstring>
#include <iostream>
#include <vector>

#define VOLK_IMPLEMENTATION
#include <volk.h>

int main(int argc, char **argv) {
  // Load the Vulkan library
  if (volkInitialize() != VK_SUCCESS) {
    std::cerr << "Failed to initialize Volk" << std::endl;
    return -1;
  }

  // Create Vulkan instance
  VkApplicationInfo appInfo = {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "CooperativeMatrixCheck";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "NoEngine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_2;

  VkInstanceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  VkInstance instance;
  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    std::cerr << "Failed to create Vulkan instance" << std::endl;
    return -1;
  }

  volkLoadInstance(instance);

  // Enumerate physical devices
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
  if (deviceCount == 0) {
    std::cerr << "No Vulkan-compatible GPU found." << std::endl;
    return -1;
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  // Check for cooperative matrix extensions
  const char *khrExtension = "VK_KHR_cooperative_matrix";
  const char *nvExtension = "VK_NV_cooperative_matrix";

  for (const auto &device : devices) {
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(device, &props);

    uint32_t extCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount,
                                         extensions.data());

    bool khrFound = false;
    bool nvFound = false;
    for (const auto &ext : extensions) {
      if (strcmp(ext.extensionName, khrExtension) == 0) {
        khrFound = true;
      }
      if (strcmp(ext.extensionName, nvExtension) == 0) {
        nvFound = true;
      }
    }

    std::cout << "Device: " << props.deviceName << "\n";
    std::cout << "  VK_KHR_cooperative_matrix: "
              << (khrFound ? "Supported" : "Not supported") << "\n";
    std::cout << "  VK_NV_cooperative_matrix: "
              << (nvFound ? "Supported" : "Not supported") << "\n";
  }

  vkDestroyInstance(instance, nullptr);
  return 0;
}
