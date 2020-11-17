#include <iostream>

#include <vulkan/vulkan.hpp>

int main(int argc, const char** argv)
{
    vk::ApplicationInfo appInfo(
            "Hello World", VK_MAKE_VERSION(1, 0, 0),
            "No Engine", VK_MAKE_VERSION(1, 0, 0),
            VK_API_VERSION_1_0
    );

    VkInstanceCreateInfo createInfo = vk::InstanceCreateInfo({}, &appInfo);

    VkInstance instance;
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

    std::cout << "Result: " << result << std::endl;

    std::cout << "Hello, World!" << std::endl;
}
