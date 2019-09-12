/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DebugName_H___
#define ___C3D_DebugName_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

#include <ashespp/Core/Device.hpp>

namespace castor3d
{
	template< typename AshesType >
	struct AshesTypeTraits;

	template<>
	struct AshesTypeTraits< ashes::Instance >
	{
		using VkType = VkInstance;
	};

	template<>
	struct AshesTypeTraits< ashes::PhysicalDevice >
	{
		using VkType = VkPhysicalDevice;
	};

	template<>
	struct AshesTypeTraits< ashes::Device >
	{
		using VkType = VkDevice;
	};

	template<>
	struct AshesTypeTraits< RenderDevice >
	{
		using VkType = VkDevice;
	};

	template<>
	struct AshesTypeTraits< ashes::Queue >
	{
		using VkType = VkQueue;
	};

	template<>
	struct AshesTypeTraits< ashes::Semaphore >
	{
		using VkType = VkSemaphore;
	};

	template<>
	struct AshesTypeTraits< ashes::CommandBuffer >
	{
		using VkType = VkCommandBuffer;
	};

	template<>
	struct AshesTypeTraits< ashes::Fence >
	{
		using VkType = VkFence;
	};

	template<>
	struct AshesTypeTraits< ashes::DeviceMemory >
	{
		using VkType = VkDeviceMemory;
	};

	template<>
	struct AshesTypeTraits< ashes::BufferBase >
	{
		using VkType = VkBuffer;
	};

	template<>
	struct AshesTypeTraits< ashes::UniformBuffer >
	{
		using VkType = VkBuffer;
	};

	template<>
	struct AshesTypeTraits< ashes::VertexBufferBase >
	{
		using VkType = VkBuffer;
	};

	template<>
	struct AshesTypeTraits< ashes::Image >
	{
		using VkType = VkImage;
	};

	template<>
	struct AshesTypeTraits< ashes::Event >
	{
		using VkType = VkEvent;
	};

	template<>
	struct AshesTypeTraits< ashes::QueryPool >
	{
		using VkType = VkQueryPool;
	};

	template<>
	struct AshesTypeTraits< ashes::BufferView >
	{
		using VkType = VkBufferView;
	};

	template<>
	struct AshesTypeTraits< ashes::ImageView >
	{
		using VkType = VkImageView;
	};

	template<>
	struct AshesTypeTraits< ashes::ShaderModule >
	{
		using VkType = VkShaderModule;
	};

	template<>
	struct AshesTypeTraits< ashes::PipelineLayout >
	{
		using VkType = VkPipelineLayout;
	};

	template<>
	struct AshesTypeTraits< ashes::RenderPass >
	{
		using VkType = VkRenderPass;
	};

	template<>
	struct AshesTypeTraits< ashes::GraphicsPipeline >
	{
		using VkType = VkPipeline;
	};

	template<>
	struct AshesTypeTraits< ashes::ComputePipeline >
	{
		using VkType = VkPipeline;
	};

	template<>
	struct AshesTypeTraits< ashes::DescriptorSetLayout >
	{
		using VkType = VkDescriptorSetLayout;
	};

	template<>
	struct AshesTypeTraits< ashes::Sampler >
	{
		using VkType = VkSampler;
	};

	template<>
	struct AshesTypeTraits< ashes::DescriptorSetPool >
	{
		using VkType = VkDescriptorPool;
	};

	template<>
	struct AshesTypeTraits< ashes::DescriptorPool >
	{
		using VkType = VkDescriptorPool;
	};

	template<>
	struct AshesTypeTraits< ashes::DescriptorSet >
	{
		using VkType = VkDescriptorSet;
	};

	template<>
	struct AshesTypeTraits< ashes::FrameBuffer >
	{
		using VkType = VkFramebuffer;
	};

	template<>
	struct AshesTypeTraits< ashes::CommandPool >
	{
		using VkType = VkCommandPool;
	};

	template<>
	struct AshesTypeTraits< ashes::Surface >
	{
		using VkType = VkSurfaceKHR;
	};

	template<>
	struct AshesTypeTraits< ashes::SwapChain >
	{
		using VkType = VkSwapchainKHR;
	};

	template< typename AshesType >
	struct AshesDebugTypeTraits;

	template<>
	struct AshesDebugTypeTraits< ashes::Instance >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::PhysicalDevice >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::Device >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< RenderDevice >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::Queue >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::Semaphore >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::CommandBuffer >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::Fence >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::DeviceMemory >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::BufferBase >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::UniformBuffer >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::VertexBufferBase >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::Image >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::Event >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::QueryPool >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_QUERY_POOL_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::BufferView >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_VIEW_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::ImageView >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::ShaderModule >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::PipelineLayout >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::RenderPass >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::GraphicsPipeline >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::ComputePipeline >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::DescriptorSetLayout >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::Sampler >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::DescriptorSetPool >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::DescriptorPool >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::DescriptorSet >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::FrameBuffer >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::CommandPool >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_POOL_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::Surface >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT;
	};

	template<>
	struct AshesDebugTypeTraits< ashes::SwapChain >
	{
		static VkDebugReportObjectTypeEXT constexpr Value = VK_DEBUG_REPORT_OBJECT_TYPE_SWAPCHAIN_KHR_EXT;
	};

	template< typename AshesType >
	void setDebugObjectName( RenderDevice const & device
		, AshesType const & object
		, std::string const & name )
	{
		using VkType = typename AshesTypeTraits< AshesType >::VkType;
		using DebugTypeTraits = typename AshesDebugTypeTraits< AshesType >;

		device.device->debugMarkerSetObjectName(
			{
				VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT,
				nullptr,
				DebugTypeTraits::Value,
				uint64_t( VkType( object ) ),
				name.c_str()
			} );
	}

	template< typename ResT >
	inline ashes::DeviceMemoryPtr setupMemory( RenderDevice const & device
		, ResT & resource
		, VkMemoryPropertyFlags flags
		, std::string const & name )
	{
		VkMemoryRequirements requirements = resource.getMemoryRequirements();
		uint32_t deduced = device.device->deduceMemoryType( requirements.memoryTypeBits
			, flags );
		auto memory = device.device->allocateMemory( VkMemoryAllocateInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, nullptr, requirements.size, deduced } );
		setDebugObjectName( device, *memory, name + "Mem" );
		return memory;
	}
}

#endif
