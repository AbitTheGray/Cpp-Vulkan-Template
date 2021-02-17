#include "vulkan_device_score.hpp"

enum class vulkanQueueUseType : uint8_t
{
    Render = 0,
    Compute = 1,
    Transfer_External = 2,
    Transfer_Internal = 3
};
static const uint8_t vulkanQueueUseTypes_Count = 4;
inline static std::array<vulkanQueueUseType, vulkanQueueUseTypes_Count> QueueUseTypes = {
        vulkanQueueUseType::Render,
        vulkanQueueUseType::Compute,
        vulkanQueueUseType::Transfer_External,
        vulkanQueueUseType::Transfer_Internal
};

struct vulkanQueueFamilies
{
public:
    struct Value
    {
        uint32_t QueueIndex;
        uint32_t AvailableCount;
    };

public:
    std::optional<Value> Graphics;
    std::optional<Value> Compute;
    // Transfer is supported on queues with Graphics or Compute support
    std::optional<Value> Transfer_External;
    std::optional<Value> Transfer_Internal;

public:
    [[nodiscard]] inline const std::optional<Value>& FromUseType(vulkanQueueUseType useType) const
    {
        switch(useType)
        {
            case vulkanQueueUseType::Render:
                return Graphics;
            case vulkanQueueUseType::Compute:
                return Compute;
            case vulkanQueueUseType::Transfer_External:
                return Transfer_External;
            case vulkanQueueUseType::Transfer_Internal:
                return Transfer_Internal;
            default:
                throw std::runtime_error("Unknown queue use type");
        }
    }

public:
    [[nodiscard]] inline bool HasAllRequired() const noexcept
    {
        return Graphics.has_value() && Compute.has_value();
    }
};
static vulkanQueueFamilies vulkan_find_queue_families(const vk::PhysicalDevice& device, int32_t* score) noexcept
{
    const std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

    bool hasOptimalGraphics = false;
    bool hasOptimalCompute = false;

    vulkanQueueFamilies indices;
    for(uint32_t i = 0; i < queueFamilies.size(); i++)
    {
        const vk::QueueFamilyProperties& queueFamily = queueFamilies[i];
        assert(queueFamily.queueCount > 0);

        bool hasGraphics = static_cast<bool>(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics);
        bool hasCompute = static_cast<bool>(queueFamily.queueFlags & vk::QueueFlagBits::eCompute);
        bool hasTransfer = static_cast<bool>(queueFamily.queueFlags & vk::QueueFlagBits::eTransfer);

        //TODO Try to not select 1 queue family more times than it supports

        // Graphics
        if(hasGraphics)
        {
            if(!indices.Graphics.has_value()) // No queue
            {
                indices.Graphics = { i, queueFamily.queueCount };
                hasOptimalGraphics = !hasCompute;
            }
            else if(!hasOptimalGraphics && !hasCompute)
            {
                indices.Graphics = { i, queueFamily.queueCount };
                hasOptimalGraphics = true;
            }
        }

        // Compute
        if(hasCompute)
        {
            if(!indices.Compute.has_value()) // No queue
            {
                indices.Compute = { i, queueFamily.queueCount };
                hasOptimalCompute = !hasGraphics;
            }
            else if(!hasOptimalCompute && !hasGraphics)
            {
                indices.Compute = { i, queueFamily.queueCount };
                hasOptimalCompute = true;
            }
        }

        // Transfer
        if(hasTransfer)
        {
            if(!indices.Transfer_External.has_value()) // No queue
            {
                // Transfer-only
                if(!hasGraphics && !hasCompute)
                {
                    indices.Transfer_External = { i, queueFamily.queueCount };
                }
            }

            if(!indices.Transfer_Internal.has_value()) // No queue
            {
                // Transfer may not be defined for Graphics or Compute queue
                // But if it explicitly says that Transfer is supported, let's guess it is the best one for in-GPU transfer
                if(hasGraphics || hasCompute)
                {
                    indices.Transfer_Internal = { i, queueFamily.queueCount };
                }
            }
        }
    }

    if(score != nullptr)
    {
        *score = 100;

        if(indices.Graphics->QueueIndex == indices.Compute->QueueIndex)
            *score -= 5;

        if(!hasOptimalGraphics)
            *score /= 2;
        if(!hasOptimalCompute)
            *score /= 2;

        if(!indices.Transfer_External.has_value())
            *score /= 2;
        if(!indices.Transfer_Internal.has_value())
            *score /= 2;

        if(!indices.Graphics.has_value())
            *score = 0;
        if(!indices.Compute.has_value())
            *score = 0;
    }

    return indices;
}

uint32_t vulkan_default_score(vk::Instance& instance, const vk::PhysicalDevice& physicalDevice, int* queueScore_ptr)
{
    uint32_t score = 0;

    vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();

    switch(properties.deviceType)
    {
        case vk::PhysicalDeviceType::eDiscreteGpu:
            score += 100;
            break;

        case vk::PhysicalDeviceType::eVirtualGpu:
        case vk::PhysicalDeviceType::eIntegratedGpu:
            score += 10;
            break;

        case vk::PhysicalDeviceType::eCpu:
            score += 1;
            break;

        case vk::PhysicalDeviceType::eOther:
            return 0; // Not suitable - Unknown physical device type
    }

    if(properties.limits.maxImageDimension2D > (1u << 11u)) // 2,048
        score++;
    if(properties.limits.maxImageDimension2D > (1u << 12u)) // 4,096
        score++;
    if(properties.limits.maxImageDimension2D > (1u << 13u)) // 8,192
        score++;
    if(properties.limits.maxImageDimension2D > (1u << 14u)) // 16,384
        score++;
    if(properties.limits.maxImageDimension2D > (1u << 15u)) // 32,768
        score++;
    if(properties.limits.maxImageDimension2D > (1u << 16u)) // 65,536
        score++;


    vk::PhysicalDeviceFeatures features = physicalDevice.getFeatures();

    int queueScore = 0;
    vulkanQueueFamilies queueIndices = vulkan_find_queue_families(physicalDevice, &queueScore);
    if(queueScore_ptr != nullptr)
        *queueScore_ptr = queueScore;
    if(queueScore <= 0)
        return 0; // Too low score
    if(!queueIndices.HasAllRequired())
        return 0; // Not suitable - Not all required queues

    if(features.samplerAnisotropy)
        score += 3;

    return score;
}
