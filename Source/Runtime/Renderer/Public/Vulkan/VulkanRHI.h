
#pragma once

namespace CK
{

struct QueueFamilyIndices {
    std::optional<uint32_t> GraphicsFamily;
    std::optional<uint32_t> PresentFamily;

    bool isComplete() {
        return GraphicsFamily.has_value() && PresentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct Vertex {
    Vector2 pos;
    Vector3 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};

class VulkanRHI : public RendererInterface
{
public:
	VulkanRHI() = default;
	~VulkanRHI();

public:
	virtual bool Init(const ScreenPoint& InScreenSize) override;
	virtual void Shutdown() override;
	virtual bool IsInitialized() const { return false; }

	virtual void Clear(const LinearColor& InClearColor) override;
	virtual void BeginFrame() override;
	virtual void EndFrame() override;

	virtual void DrawPoint(const Vector2& InVectorPos, const LinearColor& InColor) override {}
	virtual void DrawPoint(const ScreenPoint& InScreenPos, const LinearColor& InColor) override {}
	virtual void DrawLine(const Vector2& InStartPos, const Vector2& InEndPos, const LinearColor& InColor) override {}
	virtual void DrawLine(const Vector4& InStartPos, const Vector4& InEndPos, const LinearColor& InColor) override {}

	virtual float GetDepthBufferValue(const ScreenPoint& InPos) const override { return 0; }
	virtual void SetDepthBufferValue(const ScreenPoint& InPos, float InDepthValue) override {}

	virtual void DrawFullVerticalLine(int InX, const LinearColor& InColor) override {}
	virtual void DrawFullHorizontalLine(int InY, const LinearColor& InColor) override {}

	virtual void PushStatisticText(std::string && InText) override;
	virtual void PushStatisticTexts(std::vector<std::string> && InTexts) override;

    virtual void Resize(const ScreenPoint& InScreenSize) override { RecreateSwapChain(InScreenSize); }
    virtual void Draw() override { DrawFrame(); }

	virtual RendererType GetRendererType() const { return RendererType::Hardware; }

private:
    void CreateInstance();

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    bool CheckValidationLayerSupport();
    std::vector<const char*> VulkanRHI::GetRequiredExtensions();
    void SetupDebugMessenger();
    void CreateSurface();

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    bool IsDeviceSuitable(VkPhysicalDevice device);
    void PickPhysicalDevice();
    void CreateLogicalDevice();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    void CreateSwapChain();
    void CreateImageViews();
    static std::vector<char> VulkanRHI::readFile(const std::string& filename);
    VkShaderModule CreateShaderModule(const std::vector<char>& code);
    void CreateRenderPass();
    //CreateDescriptorSetLayout();
    void CreateGraphicsPipeline();
    void CreateCommandPool();
    //CreateColorResources();
    //CreateDepthResources();
    void CreateFramebuffers();
    //CreateTextureImage();
    //CreateTextureImageView();
    //CreateTextureSampler();
    //loadModel();
    UINT32 FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void CreateVertexBuffer();
    //CreateIndexBuffer();
    //CreateUniformBuffers();
    //CreateDescriptorPool();
    //CreateDescriptorSets();
    void CreateCommandBuffers();
    void CreateSyncObjects();

    void DrawFrame();
    void CleanupSwapChain();
    void RecreateSwapChain(const ScreenPoint& InScreenSize);

private:
    ScreenPoint _ScreenSize;

    VkInstance instance;
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    char* extensions[2];
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;

    bool framebufferResized = false;
    bool enableValidationLayers = true;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
};

}