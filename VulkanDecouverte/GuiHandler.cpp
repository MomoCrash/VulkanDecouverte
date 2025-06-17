#include "GuiHandler.h"

#include "Application.h"
#include "RenderWindow.h"

GuiHandler::GuiHandler() = default;

GuiHandler::~GuiHandler()
{
	for (auto pool : m_imguiPools)
	{
		vkDestroyDescriptorPool(Application::getInstance()->getDevice(), pool, nullptr);
	}
}

int GuiHandler::inject(RenderWindow* window)
{

	int index = m_imguiPools.size();
	m_imguiPools.push_back(VkDescriptorPool());

	//1: create descriptor pool for IMGUI
	// the size of the pool is very oversize, but it's copied from imgui demo itself.
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000;
	pool_info.poolSizeCount = std::size(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;
    
	if (vkCreateDescriptorPool(Application::getInstance()->getDevice(), &pool_info, nullptr, &m_imguiPools[index]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool");
	}

	// 2: initialize imgui library
	IMGUI_CHECKVERSION();
	//this initializes the core structures of imgui
	m_imguiContexts.push_back(ImGui::CreateContext());

	ImGui::SetCurrentContext(m_imguiContexts[index]);
	
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	if (m_imguiContexts.size() > 1)
	{
		io.FontDefault = m_imguiContexts[0]->Font;
	} 

	ImGui::StyleColorsDark();
	
	ImGui_ImplGlfw_InitForVulkan(window->GetWindow(), false);
    
	QueueFamilyIndices queueFamilyIndices = Application::getInstance()->findQueueFamilies(Application::getInstance()->getPhysicalDevice(), window->getSurface());
    
	ImGui_ImplVulkan_InitInfo init_info = {};
	//init_info.ApiVersion = VK_API_VERSION_1_3;              // Pass in your value of VkApplicationInfo::apiVersion, otherwise will default to header version.
	init_info.Instance = Application::getInstance()->getVulkanInstance();
	init_info.PhysicalDevice = Application::getInstance()->getPhysicalDevice();
	init_info.Device = Application::getInstance()->getDevice();
	init_info.QueueFamily = queueFamilyIndices.graphicsFamily.value();
	init_info.Queue = Application::getInstance()->getGraphicQueue();
	init_info.PipelineCache = nullptr;
	init_info.DescriptorPool = m_imguiPools[index];
	init_info.RenderPass = window->getRenderPass();
	init_info.Subpass = 0;
	init_info.MinImageCount = 2;
	init_info.ImageCount = window->MAX_FRAMES_IN_FLIGHT;
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.Allocator = nullptr;
	init_info.CheckVkResultFn = nullptr;
	ImGui_ImplVulkan_Init(&init_info);

	return index;
}

void GuiHandler::remove(int index)
{
	vkDestroyDescriptorPool(Application::getInstance()->getDevice(), m_imguiPools[index], nullptr);
	m_imguiPools.at(index) = VK_NULL_HANDLE;
	m_imguiPools.erase (m_imguiPools.begin()+index);
	m_imguiContexts.erase(m_imguiContexts.begin()+index);
}

void GuiHandler::setContext(int index)
{
	ImGui::SetCurrentContext(m_imguiContexts.at(index));
}