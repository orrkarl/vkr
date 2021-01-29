#pragma once

#include <exception>
#include <string>

#include <vulkan/vulkan.h>

namespace utils {

class VulkanError : public std::exception {
public:
    VulkanError(std::string description, VkResult status);

    const char* what() const noexcept override;
    VkResult status() const;

private:
    std::string m_desc;
    VkResult m_status;
};

} // namespace utils
