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

    // Query cooperative matrix properties if KHR extension is supported
    if (khrFound) {
      uint32_t propertyCount = 0;
      vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR(device, &propertyCount,
                                                        nullptr);

      if (propertyCount > 0) {
        std::vector<VkCooperativeMatrixPropertiesKHR> properties(propertyCount);
        for (auto &prop : properties) {
          prop.sType = VK_STRUCTURE_TYPE_COOPERATIVE_MATRIX_PROPERTIES_KHR;
          prop.pNext = nullptr;
        }

        vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR(
            device, &propertyCount, properties.data());

        std::cout << "  Cooperative Matrix Properties (" << propertyCount
                  << " supported):\n";
        for (size_t i = 0; i < properties.size(); ++i) {
          const auto &prop = properties[i];
          std::cout << "    [" << i << "] MxNxK: " << prop.MSize << "x"
                    << prop.NSize << "x" << prop.KSize << "\n";

          // Data types
          std::cout << "        A type: " << prop.AType << "\n";
          std::cout << "        B type: " << prop.BType << "\n";
          std::cout << "        C type: " << prop.CType << "\n";
          std::cout << "        Result type: " << prop.ResultType << "\n";

          // Acceleration properties
          std::cout << "        Saturating accumulation: "
                    << (prop.saturatingAccumulation ? "Yes" : "No") << "\n";
          std::cout << "        Matrix size: " << prop.MSize << "x"
                    << prop.NSize << "x" << prop.KSize << "\n";
          std::cout << "\n";
        }
      } else {
        std::cout << "  No cooperative matrix properties found\n";
      }
    }
  }

  vkDestroyInstance(instance, nullptr);
  return 0;
}
