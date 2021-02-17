#pragma once

#include <cstdint>

#include <vulkan/vulkan.hpp>

uint32_t vulkan_default_score(vk::Instance& instance, const vk::PhysicalDevice& physicalDevice, int* queueScore_ptr);
