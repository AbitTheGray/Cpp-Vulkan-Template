#pragma once

#include <iostream>

#include <vulkan/vulkan.hpp>

void print_vulkan_info()
{
    vk::ApplicationInfo appInfo = vk::ApplicationInfo(
            "Test", VK_MAKE_VERSION(0, 1, 0),
            "No Engine", VK_MAKE_VERSION(0, 1, 0),
            VK_API_VERSION_1_0
                                                     );


    vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo(
            {},
            &appInfo,
            {},
            {}
                                                              );

    vk::Instance instance = vk::createInstance(createInfo);

    std::vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();
    std::cout << "Vulkan recognized " << devices.size() << " graphical devices" << std::endl;
    for(const auto& dev : devices)
    {
        vk::PhysicalDeviceProperties prop = dev.getProperties();
        std::cout << '\t' << prop.deviceName << ":" << std::endl;

        std::cout << "\t\t- Type: ";
        switch(prop.deviceType)
        {
            default:
                std::cout << "???";
                break;
            case vk::PhysicalDeviceType::eOther:
                std::cout << "Other";
                break;
            case vk::PhysicalDeviceType::eIntegratedGpu:
                std::cout << "Integrated GPU";
                break;
            case vk::PhysicalDeviceType::eDiscreteGpu:
                std::cout << "Discrete GPU";
                break;
            case vk::PhysicalDeviceType::eVirtualGpu:
                std::cout << "Virtual GPU";
                break;
            case vk::PhysicalDeviceType::eCpu:
                std::cout << "CPU";
                break;
        }
        std::cout << std::endl;

        std::cout << "\t\t- Device ID: " << prop.deviceID << std::endl;

        // Memory
        vk::PhysicalDeviceMemoryProperties mem = dev.getMemoryProperties();

        std::size_t totalSize = 0;
        for(const auto& mh : mem.memoryHeaps)
        {
            totalSize += mh.size;
        }

        std::size_t totalSize_GiB = totalSize / (1024 * 1024 * 1024);
        std::size_t totalSize_MiB = totalSize / (1024 * 1024) - (totalSize_GiB * 1024);
        std::cout << "\t\t- Memory Size: " << totalSize_GiB << " GiB, " << totalSize_MiB << " MiB" << std::endl;
    }

    instance.destroy();
}
