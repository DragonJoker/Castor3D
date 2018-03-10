/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Core/VkDevice.hpp"

#include "Buffer/VkBuffer.hpp"
#include "Buffer/VkBufferView.hpp"
#include "Buffer/VkUniformBuffer.hpp"
#include "Command/VkCommandPool.hpp"
#include "Command/VkQueue.hpp"
#include "Core/VkConnection.hpp"
#include "Core/VkPhysicalDevice.hpp"
#include "Core/VkRenderer.hpp"
#include "Core/VkSwapChain.hpp"
#include "Descriptor/VkDescriptorPool.hpp"
#include "Descriptor/VkDescriptorSetLayout.hpp"
#include "Image/VkSampler.hpp"
#include "Image/VkTexture.hpp"
#include "Image/VkTextureView.hpp"
#include "Miscellaneous/VkDeviceMemory.hpp"
#include "Miscellaneous/VkQueryPool.hpp"
#include "Pipeline/VkPipelineLayout.hpp"
#include "RenderPass/VkRenderPass.hpp"
#include "Shader/VkAttribute.hpp"
#include "Shader/VkShaderModule.hpp"
#include "Sync/VkFence.hpp"
#include "Sync/VkSemaphore.hpp"

#include <Miscellaneous/MemoryRequirements.hpp>

namespace vk_renderer
{
	Device::Device( Renderer const & renderer
		, renderer::ConnectionPtr && connection )
		: renderer::Device{ renderer, connection->getGpu(), *connection }
		, m_renderer{ renderer }
		, m_connection{ static_cast< Connection * >( connection.release() ) }
		, m_gpu{ static_cast< PhysicalDevice const & >( renderer::Device::getPhysicalDevice() ) }
	{
		m_timestampPeriod = m_gpu.getProperties().limits.timestampPeriod;
		std::vector< VkDeviceQueueCreateInfo > queueCreateInfos;
		std::vector< float > queuePriorities = { 1.0f };

		queueCreateInfos.push_back(
		{
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,                 // sType
			nullptr,                                                    // pNext
			0,                                                          // flags
			m_connection->getGraphicsQueueFamilyIndex(),                // queueFamilyIndex
			static_cast< uint32_t >( queuePriorities.size() ),          // queueCount
			queuePriorities.data()                                      // pQueuePriorities
		} );

		if ( m_connection->getPresentQueueFamilyIndex() != m_connection->getGraphicsQueueFamilyIndex() )
		{
			queueCreateInfos.push_back(
			{
				VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,             // sType
				nullptr,                                                // pNext
				0,                                                      // flags
				m_connection->getPresentQueueFamilyIndex(),             // queueFamilyIndex
				static_cast< uint32_t >( queuePriorities.size() ),      // queueCount
				queuePriorities.data()                                  // pQueuePriorities
			} );
		}

		if ( m_connection->getComputeQueueFamilyIndex() != m_connection->getGraphicsQueueFamilyIndex() )
		{
			queueCreateInfos.push_back(
			{
				VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,             // sType
				nullptr,                                                // pNext
				0,                                                      // flags
				m_connection->getComputeQueueFamilyIndex(),             // queueFamilyIndex
				static_cast< uint32_t >( queuePriorities.size() ),      // queueCount
				queuePriorities.data()                                  // pQueuePriorities
			} );
		}

		VkDeviceCreateInfo deviceInfo
		{
			VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			nullptr,
			0,                                                                               // flags
			static_cast< uint32_t >( queueCreateInfos.size() ),                              // queueCreateInfoCount
			queueCreateInfos.data(),                                                         // pQueueCreateInfos
			static_cast< uint32_t >( m_gpu.getLayerNames().size() ),                         // enabledLayerCount
			m_gpu.getLayerNames().empty() ? nullptr : m_gpu.getLayerNames().data(),          // ppEnabledLayerNames
			static_cast< uint32_t >( m_gpu.getExtensionNames().size() ),                     // enabledExtensionCount
			m_gpu.getExtensionNames().empty() ? nullptr : m_gpu.getExtensionNames().data(),  // ppEnabledExtensionNames
			nullptr                                                                          // pEnabledFeatures
		};
		DEBUG_DUMP( deviceInfo );

		auto res = renderer.vkCreateDevice( m_gpu, &deviceInfo, nullptr, &m_device );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "LogicalDevice creation failed: " + getLastError() };
		}

#define VK_LIB_DEVICE_FUNCTION( fun ) fun = reinterpret_cast< PFN_##fun >( renderer.vkGetDeviceProcAddr( m_device, #fun ) );
#include "Miscellaneous/VulkanFunctionsList.inl"

		m_presentQueue = std::make_unique< Queue >( *this, m_connection->getPresentQueueFamilyIndex() );
		m_presentCommandPool = std::make_unique< CommandPool >( *this
			, m_presentQueue->getFamilyIndex()
			, renderer::CommandPoolCreateFlag::eResetCommandBuffer | renderer::CommandPoolCreateFlag::eTransient );

		if ( m_connection->getGraphicsQueueFamilyIndex() != m_connection->getPresentQueueFamilyIndex() )
		{
			m_graphicsQueue = std::make_unique< Queue >( *this, m_connection->getPresentQueueFamilyIndex() );
		}
		else
		{
			m_graphicsQueue = std::make_unique< Queue >( *this, m_connection->getGraphicsQueueFamilyIndex() );
		}

		m_graphicsCommandPool = std::make_unique< CommandPool >( *this
			, m_graphicsQueue->getFamilyIndex()
			, renderer::CommandPoolCreateFlag::eResetCommandBuffer | renderer::CommandPoolCreateFlag::eTransient );

		if ( m_connection->getGraphicsQueueFamilyIndex() != m_connection->getComputeQueueFamilyIndex() )
		{
			m_computeQueue = std::make_unique< Queue >( *this, m_connection->getComputeQueueFamilyIndex() );
		}
		else
		{
			m_computeQueue = std::make_unique< Queue >( *this, m_connection->getGraphicsQueueFamilyIndex() );
		}

		m_computeCommandPool = std::make_unique< CommandPool >( *this
			, m_computeQueue->getFamilyIndex()
			, renderer::CommandPoolCreateFlag::eResetCommandBuffer | renderer::CommandPoolCreateFlag::eTransient );
	}

	Device::~Device()
	{
		m_graphicsCommandPool.reset();
		m_graphicsQueue.reset();
		m_presentCommandPool.reset();
		m_presentQueue.reset();
		m_computeCommandPool.reset();
		m_computeQueue.reset();
		vkDestroyDevice( m_device, nullptr );
	}

	renderer::RenderPassPtr Device::createRenderPass( renderer::RenderPassCreateInfo createInfo )const
	{
		return std::make_unique< RenderPass >( *this, std::move( createInfo ) );
	}

	renderer::PipelineLayoutPtr Device::createPipelineLayout( renderer::DescriptorSetLayoutCRefArray const & setLayouts
			, renderer::PushConstantRangeCRefArray const & pushConstantRanges )const
	{
		return std::make_unique< PipelineLayout >( *this
			, setLayouts
			, pushConstantRanges );
	}

	renderer::DescriptorSetLayoutPtr Device::createDescriptorSetLayout( renderer::DescriptorSetLayoutBindingArray && bindings )const
	{
		return std::make_unique< DescriptorSetLayout >( *this, std::move( bindings ) );
	}

	renderer::DescriptorPoolPtr Device::createDescriptorPool( renderer::DescriptorPoolCreateFlags flags
		, uint32_t maxSets
		, renderer::DescriptorPoolSizeArray poolSizes )const
	{
		return std::make_unique< DescriptorPool >( *this, flags, maxSets, poolSizes );
	}

	renderer::DeviceMemoryPtr Device::allocateMemory( renderer::MemoryRequirements const & requirements
		, renderer::MemoryPropertyFlags flags )const
	{
		return std::make_unique< DeviceMemory >( *this
			, requirements
			, flags );
	}

	renderer::TexturePtr Device::createTexture( renderer::ImageCreateInfo const & createInfo )const
	{
		return std::make_unique< Texture >( *this, createInfo );
	}

	void Device::getImageSubresourceLayout( renderer::Texture const & image
		, renderer::ImageSubresource const & subresource
		, renderer::SubresourceLayout & layout )const
	{
		VkImageSubresource vksubresource = convert( subresource );
		VkSubresourceLayout vklayout;
		vkGetImageSubresourceLayout( m_device
			, static_cast< Texture const & >( image )
			, &vksubresource
			, &vklayout );
		layout = convert( vklayout );
	}

	renderer::SamplerPtr Device::createSampler( renderer::SamplerCreateInfo const & createInfo )const
	{
		return std::make_unique< Sampler >( *this, createInfo );
	}

	renderer::BufferBasePtr Device::createBuffer( uint32_t size
		, renderer::BufferTargets target )const
	{
		return std::make_unique< Buffer >( *this
			, size
			, target );
	}

	renderer::BufferViewPtr Device::createBufferView( renderer::BufferBase const & buffer
		, renderer::Format format
		, uint32_t offset
		, uint32_t range )const
	{
		return std::make_unique< BufferView >( *this
			, static_cast< Buffer const & >( buffer )
			, format
			, offset
			, range );
	}

	renderer::UniformBufferBasePtr Device::createUniformBuffer( uint32_t count
		, uint32_t size
		, renderer::BufferTargets target
		, renderer::MemoryPropertyFlags memoryFlags )const
	{
		return std::make_unique< UniformBuffer >( *this
			, count
			, size
			, target
			, memoryFlags );
	}

	renderer::SwapChainPtr Device::createSwapChain( renderer::Extent2D const & size )const
	{
		renderer::SwapChainPtr result;

		try
		{
			result = std::make_unique< SwapChain >( *this, size );
		}
		catch ( std::exception & exc )
		{
			std::cerr << "Could not create the swap chain:\n" << exc.what() << std::endl;
		}
		catch ( ... )
		{
			std::cerr << "Could not create the swap chain:\nUnknown error" << std::endl;
		}

		return result;
	}

	renderer::SemaphorePtr Device::createSemaphore()const
	{
		return std::make_unique< Semaphore >( *this );
	}

	renderer::FencePtr Device::createFence( renderer::FenceCreateFlags flags )const
	{
		return std::make_unique< Fence >( *this, flags );
	}

	renderer::CommandPoolPtr Device::createCommandPool( uint32_t queueFamilyIndex
		, renderer::CommandPoolCreateFlags const & flags )const
	{
		return std::make_unique< CommandPool >( *this
			, queueFamilyIndex
			, flags );
	}

	renderer::ShaderModulePtr Device::createShaderModule( renderer::ShaderStageFlag stage )const
	{
		return std::make_shared< ShaderModule >( *this, stage );
	}

	renderer::QueryPoolPtr Device::createQueryPool( renderer::QueryType type
		, uint32_t count
		, renderer::QueryPipelineStatisticFlags pipelineStatistics )const
	{
		return std::make_unique< QueryPool >( *this
			, type
			, count
			, pipelineStatistics );
	}

	void Device::waitIdle()const
	{
		vkDeviceWaitIdle( m_device );
	}

	renderer::Mat4 Device::frustum( float left
		, float right
		, float bottom
		, float top
		, float zNear
		, float zFar )const
	{
		renderer::Mat4 result( float( 0 ) );
		result[0][0] = ( float( 2 ) * zNear ) / ( right - left );
		result[1][1] = ( float( 2 ) * zNear ) / ( top - bottom );
		result[2][0] = ( right + left ) / ( right - left );
		result[2][1] = ( top + bottom ) / ( top - bottom );
		result[2][3] = float( -1 );
		result[2][2] = zFar / ( zNear - zFar );
		result[3][2] = -( zFar * zNear ) / ( zFar - zNear );

		return result;
	}

	renderer::Mat4 Device::perspective( renderer::Angle fovy
		, float aspect
		, float zNear
		, float zFar )const
	{
		float const tanHalfFovy = tan( float( fovy ) / float( 2 ) );

		renderer::Mat4 result( float( 0 ) );
		result[0][0] = float( 1 ) / ( aspect * tanHalfFovy );
		result[1][1] = float( 1 ) / ( tanHalfFovy );
		result[2][3] = -float( 1 );
		result[2][2] = zFar / ( zNear - zFar );
		result[3][2] = -( zFar * zNear ) / ( zFar - zNear );

		return result;
	}

	renderer::Mat4 Device::ortho( float left
		, float right
		, float bottom
		, float top
		, float zNear
		, float zFar )const
	{
		renderer::Mat4 result{ 1 };
		result[0][0] = float( 2 ) / ( right - left );
		result[1][1] = float( 2 ) / ( top - bottom );
		result[3][0] = -( right + left ) / ( right - left );
		result[3][1] = -( top + bottom ) / ( top - bottom );
		result[2][2] = -float( 1 ) / ( zFar - zNear );
		result[3][2] = -zNear / ( zFar - zNear );

		return result;
	}

	renderer::MemoryRequirements Device::getBufferMemoryRequirements( VkBuffer buffer )const
	{
		VkMemoryRequirements requirements;
		vkGetBufferMemoryRequirements( m_device
			, buffer
			, &requirements );
		renderer::MemoryRequirements result = convert( requirements );
		result.type = renderer::ResourceType::eBuffer;
		return result;
	}

	renderer::MemoryRequirements Device::getImageMemoryRequirements( VkImage image )const
	{
		VkMemoryRequirements requirements;
		vkGetImageMemoryRequirements( m_device
			, image
			, &requirements );
		renderer::MemoryRequirements result = convert( requirements );
		result.type = renderer::ResourceType::eImage;
		return result;
	}
}
