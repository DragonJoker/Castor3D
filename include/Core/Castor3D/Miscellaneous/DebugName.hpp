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
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_INSTANCE;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkInstance" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::PhysicalDevice >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_PHYSICAL_DEVICE;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkPhysicalDevice" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::Device >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_PHYSICAL_DEVICE;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkDevice" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< RenderDevice >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_PHYSICAL_DEVICE;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "C3dRenderDevice" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::Queue >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_QUEUE;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkQueue" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::Semaphore >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_SEMAPHORE;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkSemaphore" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::CommandBuffer >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_COMMAND_BUFFER;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkCommandBuffer" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::Fence >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_FENCE;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkFence" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::DeviceMemory >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_DEVICE_MEMORY;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkDeviceMemory" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::BufferBase >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_BUFFER;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkBuffer" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::UniformBuffer >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_BUFFER;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkUniformBuffer" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::VertexBufferBase >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_BUFFER;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkVertexBuffer" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::Image >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_IMAGE;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkImage" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::Event >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_EVENT;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkEvent" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::QueryPool >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_QUERY_POOL;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_QUERY_POOL_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkQueryPool" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::BufferView >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_BUFFER_VIEW;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_VIEW_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkBufferView" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::ImageView >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_IMAGE_VIEW;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkImageView" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::ShaderModule >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_SHADER_MODULE;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkShaderModule" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::PipelineLayout >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_PIPELINE_LAYOUT;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkPipelineLayout" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::RenderPass >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_RENDER_PASS;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkRenderPass" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::GraphicsPipeline >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_PIPELINE;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkGraphicsPipeline" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::ComputePipeline >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_PIPELINE;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkComputePipeline" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::DescriptorSetLayout >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkDescriptorSetLayout" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::Sampler >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_SAMPLER;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkSampler" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::DescriptorSetPool >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_DESCRIPTOR_POOL;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkDescriptorSetPool" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::DescriptorPool >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_DESCRIPTOR_POOL;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkDecriptorPool" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::DescriptorSet >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_DESCRIPTOR_SET;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkDescriptorSet" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::FrameBuffer >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_FRAMEBUFFER;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkFramebuffer" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::CommandPool >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_COMMAND_POOL;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_POOL_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkCommandPool" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::Surface >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_SURFACE_KHR;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkSurfaceKHR" };
			return result;
		}
	};

	template<>
	struct AshesDebugTypeTraits< ashes::SwapChain >
	{
#if VK_EXT_debug_utils
		static VkObjectType constexpr UtilsValue = VK_OBJECT_TYPE_SWAPCHAIN_KHR;
#endif
#if VK_EXT_debug_report || VK_EXT_debug_marker
		static VkDebugReportObjectTypeEXT constexpr ReportValue = VK_DEBUG_REPORT_OBJECT_TYPE_SWAPCHAIN_KHR_EXT;
#endif
		static std::string const & getName()
		{
			static std::string result{ "VkSwapchainKHR" };
			return result;
		}
	};

	template< typename AshesType >
	void setDebugObjectName( RenderDevice const & device
		, AshesType const & object
		, std::string const & name )
	{
		using VkType = typename AshesTypeTraits< AshesType >::VkType;
		using DebugTypeTraits = typename AshesDebugTypeTraits< AshesType >;
		castor::Logger::logTrace( "Created object [" + name + "] of type " + DebugTypeTraits::getName() );

#if VK_EXT_debug_utils

		if ( device.device->hasDebugUtils() )
		{
			device.device->setDebugUtilsObjectName(
				{
					VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
					nullptr,
					DebugTypeTraits::UtilsValue,
					uint64_t( VkType( object ) ),
					name.c_str()
				} );
		}
		else

#endif
#if VK_EXT_debug_marker

		if ( device.device->hasDebugMarker() )
		{
			device.device->debugMarkerSetObjectName(
				{
					VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT,
					nullptr,
					DebugTypeTraits::ReportValue,
					uint64_t( VkType( object ) ),
					name.c_str()
				} );
		}

#endif
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
