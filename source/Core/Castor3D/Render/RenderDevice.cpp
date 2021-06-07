#include "Castor3D/Render/RenderDevice.hpp"

#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"

#include <ashespp/Core/Instance.hpp>

#include <RenderGraph/GraphContext.hpp>

CU_ImplementCUSmartPtr( castor3d, RenderDevice )

namespace castor3d
{
	namespace
	{
		RenderDevice::QueueFamilies initialiseQueueFamilies( ashes::Instance const & instance
			, ashes::PhysicalDevice const & gpu )
		{
			RenderDevice::QueueFamilies result;
			auto queueProps = gpu.getQueueFamilyProperties();
			bool hasGraphics = false;
			uint32_t index{};

			for ( auto & queueProp : queueProps )
			{
				if ( queueProp.queueCount > 0 )
				{
					RenderDevice::QueueData data;
					data.familyIndex = index++;

					if ( ashes::checkFlag( queueProp.queueFlags, VK_QUEUE_GRAPHICS_BIT ) )
					{
						hasGraphics = true;
						data.familySupport |= QueueFamilyFlag::eGraphics;
					}

					if ( ashes::checkFlag( queueProp.queueFlags, VK_QUEUE_COMPUTE_BIT ) )
					{
						data.familySupport |= QueueFamilyFlag::eCompute;
					}

					if ( ashes::checkFlag( queueProp.queueFlags, VK_QUEUE_TRANSFER_BIT ) )
					{
						data.familySupport |= QueueFamilyFlag::eTransfer;
					}

					data.queues.resize( queueProp.queueCount );

					if ( data.familySupport != QueueFamilyFlag::eNone )
					{
						result.push_back( std::move( data ) );
					}
				}
			}

			// Si on n'en a pas trouvé, on génère une erreur.
			if ( !hasGraphics )
			{
				throw ashes::Exception{ VK_ERROR_INITIALIZATION_FAILED
					, "No queue with graphics support was found" };
			}

			return result;
		}

		ashes::DeviceQueueCreateInfoArray getQueueCreateInfos( RenderDevice::QueueFamilies const & queues )
		{
			ashes::DeviceQueueCreateInfoArray queueCreateInfos;

			for ( auto & queueData : queues )
			{
				std::vector< float > queuePriorities;
				queuePriorities.resize( queueData.queues.size(), 1.0f );

				queueCreateInfos.push_back( { 0u
					, queueData.familyIndex
					, queuePriorities } );
			}

			return queueCreateInfos;
		}

		ashes::DeviceCreateInfo getDeviceCreateInfo( ashes::Instance const & instance
			, ashes::PhysicalDevice const & gpu
			, ashes::DeviceQueueCreateInfoArray queueCreateInfos )
		{
			log::debug << "Instance enabled layers count: " << uint32_t( instance.getEnabledLayerNames().size() ) << std::endl;
			ashes::StringArray extensions = { std::string{ VK_KHR_SWAPCHAIN_EXTENSION_NAME } };
			return ashes::DeviceCreateInfo{ 0u
				, std::move( queueCreateInfos )
				, instance.getEnabledLayerNames()
				, extensions
				, gpu.getFeatures() };
		}
	}

	RenderDevice::RenderDevice( RenderSystem & renderSystem
		, ashes::PhysicalDevice const & gpu
		, AshPluginDescription const & desc )
		: renderSystem{ renderSystem }
		, gpu{ gpu }
		, desc{ desc }
		, memoryProperties{ gpu.getMemoryProperties() }
		, properties{ gpu.getProperties() }
		, features{ gpu.getFeatures() }
		, queueFamilies{ initialiseQueueFamilies( renderSystem.getInstance(), gpu ) }
		, device{ renderSystem.getInstance().createDevice( gpu
			, getDeviceCreateInfo( renderSystem.getInstance()
				, gpu
				, getQueueCreateInfos( queueFamilies ) ) ) }
	{
		for ( auto & queueData : queueFamilies )
		{
			queueData.commandPool = device->createCommandPool( queueData.familyIndex
				, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT );
			uint32_t index = 0u;

			for ( auto & queue : queueData.queues )
			{
				queue = device->getQueue( queueData.familyIndex, index++ );
			}

			if ( !m_preferredGraphicsQueue
				&& checkFlag( queueData.familySupport, QueueFamilyFlag::eGraphics ) )
			{
				m_preferredGraphicsQueue = &queueData;
				graphicsCommandPool = queueData.commandPool.get();
				graphicsQueue = queueData.queues.front().get();
			}

			if ( !m_preferredComputeQueue
				&& checkFlag( queueData.familySupport, QueueFamilyFlag::eCompute ) )
			{
				m_preferredComputeQueue = &queueData;
				computeCommandPool = queueData.commandPool.get();
				computeQueue = queueData.queues.front().get();
			}

			if ( !m_preferredTransferQueue
				&& checkFlag( queueData.familySupport, QueueFamilyFlag::eTransfer ) )
			{
				m_preferredTransferQueue = &queueData;
				transferCommandPool = queueData.commandPool.get();
				transferQueue = queueData.queues.front().get();
			}
		}

		bufferPool = std::make_shared< GpuBufferPool >( renderSystem, *this, cuT( "GlobalBufferPool" ) );
		uboPools = std::make_shared< UniformBufferPools >( renderSystem, *this );
	}

	RenderDevice::~RenderDevice()
	{
		renderSystem.getEngine()->getGraphResourceHandler().clear( makeContext() );
		uboPools.reset();
		bufferPool.reset();
		queueFamilies.clear();
		device.reset();
	}

	VkFormat RenderDevice::selectSuitableDepthFormat( VkFormatFeatureFlags requiredFeatures )const
	{
		std::vector< VkFormat > depthFormats
		{
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_X8_D24_UNORM_PACK32,
			VK_FORMAT_D16_UNORM,
		};
		return selectSuitableFormat( depthFormats, requiredFeatures );
	}

	VkFormat RenderDevice::selectSuitableStencilFormat( VkFormatFeatureFlags requiredFeatures )const
	{
		std::vector< VkFormat > depthFormats
		{
			VK_FORMAT_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D32_SFLOAT_S8_UINT,
		};
		return selectSuitableFormat( depthFormats, requiredFeatures );
	}

	VkFormat RenderDevice::selectSuitableDepthStencilFormat( VkFormatFeatureFlags requiredFeatures )const
	{
		std::vector< VkFormat > depthFormats
		{
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
		};
		return selectSuitableFormat( depthFormats, requiredFeatures );
	}

	VkFormat RenderDevice::selectSuitableFormat( std::vector< VkFormat > const & formats
		, VkFormatFeatureFlags requiredFeatures )const
	{
		auto it = std::find_if( formats.begin()
			, formats.end()
			, [this, &requiredFeatures]( VkFormat lookup )
			{
				auto props = device->getPhysicalDevice().getFormatProperties( lookup );
				return castor::checkFlag( props.optimalTilingFeatures, requiredFeatures );
			} );

		if ( it == formats.end() )
		{
			CU_Exception( "Could not find a suitable format." );
		}

		return *it;
	}

	crg::GraphContext RenderDevice::makeContext()const
	{
		return { *device
			, VK_NULL_HANDLE
			, device->getAllocationCallbacks()
			, device->getMemoryProperties()
			, device->getProperties()
			, false
			, device->vkGetDeviceProcAddr };
	}
}
