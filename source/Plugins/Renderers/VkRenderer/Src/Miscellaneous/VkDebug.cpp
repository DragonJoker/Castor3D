/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	std::multimap< std::string, std::string > Debug::m_dump;

	template<> std::string const VkTypeNamer< VkApplicationInfo >::Name = "VkApplicationInfo";
	template<> std::string const VkTypeNamer< VkAttachmentDescription >::Name = "VkAttachmentDescription";
	template<> std::string const VkTypeNamer< VkAttachmentReference >::Name = "VkAttachmentReference";
	template<> std::string const VkTypeNamer< VkBufferCopy >::Name = "VkBufferCopy";
	template<> std::string const VkTypeNamer< VkBufferCreateInfo >::Name = "VkBufferCreateInfo";
	template<> std::string const VkTypeNamer< VkBufferImageCopy >::Name = "VkBufferImageCopy";
	template<> std::string const VkTypeNamer< VkBufferMemoryBarrier >::Name = "VkBufferMemoryBarrier";
	template<> std::string const VkTypeNamer< VkBufferViewCreateInfo >::Name = "VkBufferViewCreateInfo";
	template<> std::string const VkTypeNamer< VkClearColorValue >::Name = "VkClearColorValue";
	template<> std::string const VkTypeNamer< VkClearDepthStencilValue >::Name = "VkClearDepthStencilValue";
	template<> std::string const VkTypeNamer< VkClearValue >::Name = "VkClearValue";
	template<> std::string const VkTypeNamer< VkCommandBufferAllocateInfo >::Name = "VkCommandBufferAllocateInfo";
	template<> std::string const VkTypeNamer< VkCommandBufferBeginInfo >::Name = "VkCommandBufferBeginInfo";
	template<> std::string const VkTypeNamer< VkCommandBufferInheritanceInfo >::Name = "VkCommandBufferInheritanceInfo";
	template<> std::string const VkTypeNamer< VkCommandPoolCreateInfo >::Name = "VkCommandPoolCreateInfo";
	template<> std::string const VkTypeNamer< VkComponentMapping >::Name = "VkComponentMapping";
	template<> std::string const VkTypeNamer< VkComputePipelineCreateInfo >::Name = "VkComputePipelineCreateInfo";
	template<> std::string const VkTypeNamer< VkDescriptorBufferInfo >::Name = "VkDescriptorBufferInfo";
	template<> std::string const VkTypeNamer< VkDescriptorImageInfo >::Name = "VkDescriptorImageInfo";
	template<> std::string const VkTypeNamer< VkDescriptorPoolCreateInfo >::Name = "VkDescriptorPoolCreateInfo";
	template<> std::string const VkTypeNamer< VkDescriptorPoolSize >::Name = "VkDescriptorPoolSize";
	template<> std::string const VkTypeNamer< VkDescriptorSetAllocateInfo >::Name = "VkDescriptorSetAllocateInfo";
	template<> std::string const VkTypeNamer< VkDescriptorSetLayoutBinding >::Name = "VkDescriptorSetLayoutBinding";
	template<> std::string const VkTypeNamer< VkDescriptorSetLayoutCreateInfo >::Name = "VkDescriptorSetLayoutCreateInfo";
	template<> std::string const VkTypeNamer< VkDeviceCreateInfo >::Name = "VkDeviceCreateInfo";
	template<> std::string const VkTypeNamer< VkDeviceQueueCreateInfo >::Name = "VkDeviceQueueCreateInfo";
	template<> std::string const VkTypeNamer< VkExtensionProperties >::Name = "VkExtensionProperties";
	template<> std::string const VkTypeNamer< VkExtent2D >::Name = "VkExtent2D";
	template<> std::string const VkTypeNamer< VkExtent3D >::Name = "VkExtent3D";
	template<> std::string const VkTypeNamer< VkFenceCreateInfo >::Name = "VkFenceCreateInfo";
	template<> std::string const VkTypeNamer< VkFramebufferCreateInfo >::Name = "VkFramebufferCreateInfo";
	template<> std::string const VkTypeNamer< VkGraphicsPipelineCreateInfo >::Name = "VkGraphicsPipelineCreateInfo";
	template<> std::string const VkTypeNamer< VkImageBlit >::Name = "VkImageBlit";
	template<> std::string const VkTypeNamer< VkImageCopy >::Name = "VkImageCopy";
	template<> std::string const VkTypeNamer< VkImageCreateInfo >::Name = "VkImageCreateInfo";
	template<> std::string const VkTypeNamer< VkImageMemoryBarrier >::Name = "VkImageMemoryBarrier";
	template<> std::string const VkTypeNamer< VkImageSubresourceLayers >::Name = "VkImageSubresourceLayers";
	template<> std::string const VkTypeNamer< VkImageSubresourceRange >::Name = "VkImageSubresourceRange";
	template<> std::string const VkTypeNamer< VkImageViewCreateInfo >::Name = "VkImageViewCreateInfo";
	template<> std::string const VkTypeNamer< VkInstanceCreateInfo >::Name = "VkInstanceCreateInfo";
	template<> std::string const VkTypeNamer< VkLayerProperties >::Name = "VkLayerProperties";
	template<> std::string const VkTypeNamer< VkMappedMemoryRange >::Name = "VkMappedMemoryRange";
	template<> std::string const VkTypeNamer< VkMemoryAllocateInfo >::Name = "VkMemoryAllocateInfo";
	template<> std::string const VkTypeNamer< VkOffset2D >::Name = "VkOffset2D";
	template<> std::string const VkTypeNamer< VkOffset3D >::Name = "VkOffset3D";
	template<> std::string const VkTypeNamer< VkPhysicalDeviceFeatures >::Name = "VkPhysicalDeviceFeatures";
	template<> std::string const VkTypeNamer< VkPipelineColorBlendAttachmentState >::Name = "VkPipelineColorBlendAttachmentState";
	template<> std::string const VkTypeNamer< VkPipelineColorBlendStateCreateInfo >::Name = "VkPipelineColorBlendStateCreateInfo";
	template<> std::string const VkTypeNamer< VkPipelineDepthStencilStateCreateInfo >::Name = "VkPipelineDepthStencilStateCreateInfo";
	template<> std::string const VkTypeNamer< VkPipelineDynamicStateCreateInfo >::Name = "VkPipelineDynamicStateCreateInfo";
	template<> std::string const VkTypeNamer< VkPipelineInputAssemblyStateCreateInfo >::Name = "VkPipelineInputAssemblyStateCreateInfo";
	template<> std::string const VkTypeNamer< VkPipelineLayoutCreateInfo >::Name = "VkPipelineLayoutCreateInfo";
	template<> std::string const VkTypeNamer< VkPipelineMultisampleStateCreateInfo >::Name = "VkPipelineMultisampleStateCreateInfo";
	template<> std::string const VkTypeNamer< VkPipelineRasterizationStateCreateInfo >::Name = "VkPipelineRasterizationStateCreateInfo";
	template<> std::string const VkTypeNamer< VkPipelineShaderStageCreateInfo >::Name = "VkPipelineShaderStageCreateInfo";
	template<> std::string const VkTypeNamer< VkPipelineTessellationStateCreateInfo >::Name = "VkPipelineTessellationStateCreateInfo";
	template<> std::string const VkTypeNamer< VkPipelineVertexInputStateCreateInfo >::Name = "VkPipelineVertexInputStateCreateInfo";
	template<> std::string const VkTypeNamer< VkPipelineViewportStateCreateInfo >::Name = "VkPipelineViewportStateCreateInfo";
	template<> std::string const VkTypeNamer< VkPresentInfoKHR >::Name = "VkPresentInfoKHR";
	template<> std::string const VkTypeNamer< VkPushConstantRange >::Name = "VkPushConstantRange";
	template<> std::string const VkTypeNamer< VkQueryPoolCreateInfo >::Name = "VkQueryPoolCreateInfo";
	template<> std::string const VkTypeNamer< VkRect2D >::Name = "VkRect2D";
	template<> std::string const VkTypeNamer< VkRenderPassBeginInfo >::Name = "VkRenderPassBeginInfo";
	template<> std::string const VkTypeNamer< VkRenderPassCreateInfo >::Name = "VkRenderPassCreateInfo";
	template<> std::string const VkTypeNamer< VkSamplerCreateInfo >::Name = "VkSamplerCreateInfo";
	template<> std::string const VkTypeNamer< VkSemaphoreCreateInfo >::Name = "VkSemaphoreCreateInfo";
	template<> std::string const VkTypeNamer< VkShaderModuleCreateInfo >::Name = "VkShaderModuleCreateInfo";
	template<> std::string const VkTypeNamer< VkSpecializationInfo >::Name = "VkSpecializationInfo";
	template<> std::string const VkTypeNamer< VkSpecializationMapEntry >::Name = "VkSpecializationMapEntry";
	template<> std::string const VkTypeNamer< VkStencilOpState >::Name = "VkStencilOpState";
	template<> std::string const VkTypeNamer< VkSubmitInfo >::Name = "VkSubmitInfo";
	template<> std::string const VkTypeNamer< VkSubpassDependency >::Name = "VkSubpassDependency";
	template<> std::string const VkTypeNamer< VkSubpassDescription >::Name = "VkSubpassDescription";
	template<> std::string const VkTypeNamer< VkSwapchainCreateInfoKHR >::Name = "VkSwapchainCreateInfoKHR";
	template<> std::string const VkTypeNamer< VkVertexInputAttributeDescription >::Name = "VkVertexInputAttributeDescription";
	template<> std::string const VkTypeNamer< VkVertexInputBindingDescription >::Name = "VkVertexInputBindingDescription";
	template<> std::string const VkTypeNamer< VkViewport >::Name = "VkViewport";
	template<> std::string const VkTypeNamer< VkWriteDescriptorSet >::Name = "VkWriteDescriptorSet";

#ifdef VK_USE_PLATFORM_WIN32_KHR

	template<> std::string const VkTypeNamer< VkWin32SurfaceCreateInfoKHR >::Name = "VkWin32SurfaceCreateInfoKHR";

#endif
}
