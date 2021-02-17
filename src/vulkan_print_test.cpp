#include "vulkan_print_test.hpp"

#include <iostream>

#include <vulkan/vulkan.hpp>

#include "vulkan_device_score.hpp"

static std::ostream& writeSize(std::ostream& out, std::size_t total)
{
    const std::size_t TiB = 1024ull * 1024ull * 1024ull * 1024ull;
    const std::size_t GiB = 1024ull * 1024ull * 1024ull;
    const std::size_t MiB = 1024ull * 1024ull;
    const std::size_t KiB = 1024ull;

    if(total == 0)
    {
        out << "NONE";
        return out;
    }

    std::size_t totalSize_TiB = total / TiB;
    total -= totalSize_TiB * TiB;

    std::size_t totalSize_GiB = total / GiB;
    total -= totalSize_GiB * GiB;

    std::size_t totalSize_MiB = total / MiB;
    total -= totalSize_MiB * MiB;

    std::size_t totalSize_KiB = total / KiB;
    total -= totalSize_KiB * KiB;

    if(totalSize_TiB != 0)
        out << totalSize_TiB << " TiB ";
    if(totalSize_GiB != 0)
        out << totalSize_GiB << " GiB ";
    if(totalSize_MiB != 0)
        out << totalSize_MiB << " MiB ";
    if(totalSize_KiB != 0)
        out << totalSize_KiB << " KiB ";
    if(total != 0)
        out << total << " B ";

    return out;
}

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

        std::cout << "\t\t- ID: " << std::endl;
        std::cout << "\t\t\t- Vendor: " << prop.vendorID << std::endl;
        std::cout << "\t\t\t- Device: " << prop.deviceID << std::endl;

        int queueScore;
        std::cout << "\t\t- Custom Score: " << vulkan_default_score(instance, dev, &queueScore) << std::endl;
        std::cout << "\t\t\t- Queue: " << queueScore << std::endl;

        // Memory
        vk::PhysicalDeviceMemoryProperties mem = dev.getMemoryProperties();

        std::size_t totalSize = 0;
        std::size_t totalSize_local = 0;
        std::size_t totalSize_mi = 0;
        for(std::size_t i = 0; i < mem.memoryHeapCount; i++)
        {
            const auto& mh = mem.memoryHeaps[i];
            totalSize += mh.size;

            if(mh.flags & vk::MemoryHeapFlagBits::eDeviceLocal)
                totalSize_local += mh.size;
            if(mh.flags & vk::MemoryHeapFlagBits::eMultiInstance)
                totalSize_mi += mh.size;
        }

        std::cout << "\t\t- Memory: " << std::endl;
        {
            std::cout << "\t\t\t- Heaps: " << std::endl;

            std::cout << "\t\t\t\t- Count: " << mem.memoryHeapCount << std::endl;

            writeSize(std::cout << "\t\t\t\t- Total: ", totalSize) << std::endl;
            writeSize(std::cout << "\t\t\t\t- Local: ", totalSize_local) << std::endl;
            writeSize(std::cout << "\t\t\t\t- Multi-instance: ", totalSize_mi) << std::endl;

            std::cout << "\t\t\t- Types: " << mem.memoryTypeCount << std::endl;
            std::vector<vk::MemoryType> memoryTypes(mem.memoryTypeCount);
            for(std::size_t i = 0; i < mem.memoryTypeCount; i++)
                memoryTypes[i] = mem.memoryTypes[i];
            std::sort(
                    memoryTypes.begin(),
                    memoryTypes.end(),
                    [](const vk::MemoryType& left, const vk::MemoryType& right) -> bool { return left.heapIndex < right.heapIndex; }
            );
            for(const auto& mt : memoryTypes)
            {
                std::cout << "\t\t\t\t" << mt.heapIndex << std::endl;

                std::cout << "\t\t\t\t\tCPU:";
                if(
                        mt.propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible ||
                        mt.propertyFlags & vk::MemoryPropertyFlagBits::eHostCoherent ||
                        mt.propertyFlags & vk::MemoryPropertyFlagBits::eHostCached
                        )
                {
                    if(mt.propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible)
                        std::cout << " Visible";
                    if(mt.propertyFlags & vk::MemoryPropertyFlagBits::eHostCoherent)
                        std::cout << " Coherent";
                    if(mt.propertyFlags & vk::MemoryPropertyFlagBits::eHostCached)
                        std::cout << " Cached";
                }
                std::cout << std::endl;

                std::cout << "\t\t\t\t\tGPU:";
                if(
                        mt.propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal ||
                        mt.propertyFlags & vk::MemoryPropertyFlagBits::eDeviceCoherentAMD ||
                        mt.propertyFlags & vk::MemoryPropertyFlagBits::eDeviceUncachedAMD
                        )
                {
                    if(mt.propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal)
                        std::cout << " Local";
                    if(mt.propertyFlags & vk::MemoryPropertyFlagBits::eDeviceCoherentAMD)
                        std::cout << " Coherent(AMD)";
                    if(mt.propertyFlags & vk::MemoryPropertyFlagBits::eDeviceUncachedAMD)
                        std::cout << " Uncached(AMD)";
                }
                std::cout << std::endl;

                if(
                        mt.propertyFlags & vk::MemoryPropertyFlagBits::eLazilyAllocated ||
                        mt.propertyFlags & vk::MemoryPropertyFlagBits::eProtected
                        )
                {
                    std::cout << "\t\t\t\t\tOther:";
                    if(mt.propertyFlags & vk::MemoryPropertyFlagBits::eLazilyAllocated)
                        std::cout << " Lazy";
                    if(mt.propertyFlags & vk::MemoryPropertyFlagBits::eProtected)
                        std::cout << " Protected";
                    std::cout << std::endl;
                }

            }
        }

        std::cout << "\t\t- Queues: " << std::endl;
        {
            const std::vector<vk::QueueFamilyProperties> queueFamilies = dev.getQueueFamilyProperties();
            for(std::size_t i = 0; i < queueFamilies.size(); i++)
            {
                std::cout << "\t\t\t" << i << std::endl;
                const vk::QueueFamilyProperties& queueFamily = queueFamilies[i];

                std::cout << "\t\t\t\tCount: " << queueFamily.queueCount << std::endl;
                std::cout << "\t\t\t\tTimestamp Valid Bits: " << queueFamily.timestampValidBits << std::endl;
                std::cout << "\t\t\t\tImage Transfer Granularity: " << queueFamily.minImageTransferGranularity.width << " x " << queueFamily.minImageTransferGranularity.height << " x " << queueFamily.minImageTransferGranularity.depth << std::endl;

                std::cout << "\t\t\t\tFlags: " << vk::to_string(queueFamily.queueFlags) << std::endl;
            }


        }
    }

    instance.destroy();
}
