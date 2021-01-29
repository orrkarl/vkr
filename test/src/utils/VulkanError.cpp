#include "VulkanError.h"

namespace utils {

VulkanError::VulkanError(std::string description, VkResult status)
    : m_desc(description + ": " + std::to_string(status)), m_status(status) {
}

const char* VulkanError::what() const noexcept {
    return m_desc.c_str();
}

VkResult VulkanError::status() const {
    return m_status;
}

} // namespace utils
