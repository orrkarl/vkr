#include <iostream>
#include <stdexcept>
#include <cstdlib>


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"


#include "predefs.h"
#include "vk_util.h"

const int WIDTH = 800;
const int HEIGHT = 600;

class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, g_AppName, nullptr, nullptr);
    }

    void initVulkan() {
        if (g_enableValidationLayers && !checkValidationLayerSupport(g_validationLayers))
        {
            throw std::runtime_error("validation which are required could not be found");
        }

        setupInstance();
        setupDebugCallback();
    }

    void setupInstance()
    {
        /*VkApplicationInfo app_info{};
        app_info.pApplicationName = g_AppName;
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = g_AppName;
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion = VK_API_VERSION_1_0;
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pNext = nullptr;

        VkInstanceCreateInfo inst_info{};
        inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        inst_info.pApplicationInfo = &app_info;

        auto extensions = getRequiredExtensions(g_enableValidationLayers);

        inst_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        inst_info.ppEnabledExtensionNames = extensions.data();

        if (g_enableValidationLayers)
        {
            inst_info.enabledLayerCount = static_cast<uint32_t>(g_validationLayers.size());
            inst_info.ppEnabledExtensionNames = g_validationLayers.data();
        }
        else
        {
            inst_info.enabledLayerCount = 0;
        }

        CheckError(vkCreateInstance(&inst_info, nullptr, &instance), "could not create instance");*/
        if (g_enableValidationLayers && !checkValidationLayerSupport(g_validationLayers)) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions(g_enableValidationLayers);
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        if (g_enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(g_validationLayers.size());
            createInfo.ppEnabledLayerNames = g_validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        CheckError(vkCreateInstance(&createInfo, nullptr, &instance), "could not create instance");
    }

    void setupDebugCallback()
    {
        if (!g_enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;

        if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &callback) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to set up debug callback!");
        }
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        if (g_enableValidationLayers)
        {
            DestroyDebugUtilsMessengerEXT(instance, callback, nullptr);
        }

        glfwDestroyWindow(window);
        glfwTerminate();
        vkDestroyInstance(instance, nullptr);
    }
private:
    VkInstance instance;
    VkDebugUtilsMessengerEXT callback;
};

int main() {
    HelloTriangleApplication app;
    
    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}