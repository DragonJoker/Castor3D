#include "Castor3D/Render/RenderDevice.hpp"

#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"

#include <ashespp/Core/Instance.hpp>

namespace castor3d
{
	namespace
	{
		std::array< uint32_t, 4u > initialiseQueueFamilies( ashes::Instance const & instance
			, ashes::Surface const & surface
			, ashes::PhysicalDevice const & gpu )
		{
			std::array< uint32_t, 4u > queueFamiliesIndex;
			// Parcours des propriétés des files, pour vérifier leur support de la présentation.
			auto queueProps = gpu.getQueueFamilyProperties();
			std::vector< uint32_t > supportsPresent( static_cast< uint32_t >( queueProps.size() ) );
			uint32_t i{ 0u };
			queueFamiliesIndex[RenderDevice::GraphicsIdx] = std::numeric_limits< uint32_t >::max();
			queueFamiliesIndex[RenderDevice::PresentIdx] = std::numeric_limits< uint32_t >::max();
			queueFamiliesIndex[RenderDevice::ComputeIdx] = std::numeric_limits< uint32_t >::max();
			queueFamiliesIndex[RenderDevice::TransferIdx] = std::numeric_limits< uint32_t >::max();

			for ( auto & present : supportsPresent )
			{
				present = surface.getSupport( i );

				if ( queueProps[i].queueCount > 0 )
				{
					if ( ashes::checkFlag( queueProps[i].queueFlags, VK_QUEUE_GRAPHICS_BIT ) )
					{
						// Tout d'abord on choisit une file graphique
						if ( queueFamiliesIndex[RenderDevice::GraphicsIdx] == std::numeric_limits< uint32_t >::max() )
						{
							queueFamiliesIndex[RenderDevice::GraphicsIdx] = i;
							queueFamiliesIndex[RenderDevice::ComputeIdx] = i;
							queueFamiliesIndex[RenderDevice::TransferIdx] = i;
						}

						// Si une file supporte les graphismes et la présentation, on la préfère.
						if ( present )
						{
							queueFamiliesIndex[RenderDevice::GraphicsIdx] = i;
							queueFamiliesIndex[RenderDevice::PresentIdx] = i;
							break;
						}
					}
					else if ( ashes::checkFlag( queueProps[i].queueFlags, VK_QUEUE_COMPUTE_BIT )
						&& queueFamiliesIndex[RenderDevice::ComputeIdx] == std::numeric_limits< uint32_t >::max() )
					{
						queueFamiliesIndex[RenderDevice::ComputeIdx] = i;
					}
					else if ( ashes::checkFlag( queueProps[i].queueFlags, VK_QUEUE_TRANSFER_BIT )
						&& queueFamiliesIndex[RenderDevice::TransferIdx] == std::numeric_limits< uint32_t >::max() )
					{
						queueFamiliesIndex[RenderDevice::TransferIdx] = i;
					}
				}

				++i;
			}

			if ( queueFamiliesIndex[RenderDevice::PresentIdx] == std::numeric_limits< uint32_t >::max() )
			{
				// Pas de file supportant les deux, on a donc 2 files distinctes.
				for ( size_t i = 0; i < queueProps.size(); ++i )
				{
					if ( supportsPresent[i] )
					{
						queueFamiliesIndex[RenderDevice::PresentIdx] = static_cast< uint32_t >( i );
						break;
					}
				}
			}

			// Si on n'en a pas trouvé, on génère une erreur.
			if ( queueFamiliesIndex[RenderDevice::GraphicsIdx] == std::numeric_limits< uint32_t >::max()
				|| queueFamiliesIndex[RenderDevice::PresentIdx] == std::numeric_limits< uint32_t >::max()
				|| queueFamiliesIndex[RenderDevice::ComputeIdx] == std::numeric_limits< uint32_t >::max()
				|| queueFamiliesIndex[RenderDevice::TransferIdx] == std::numeric_limits< uint32_t >::max() )
			{
				throw ashes::Exception{ VK_ERROR_INITIALIZATION_FAILED
					, "Queue families retrieval" };
			}

			return queueFamiliesIndex;
		}

		ashes::DeviceQueueCreateInfoArray getQueueCreateInfos( std::array< uint32_t, 4u > const & queueFamiliesIndex )
		{
			ashes::DeviceQueueCreateInfoArray queueCreateInfos;
			std::vector< float > queuePriorities = { 1.0f };

			if ( queueFamiliesIndex[RenderDevice::GraphicsIdx] != uint32_t( InvalidIndex ) )
			{
				queueCreateInfos.push_back(
					{
						0u,
						queueFamiliesIndex[RenderDevice::GraphicsIdx],
						queuePriorities,
					} );
			}

			if ( queueFamiliesIndex[RenderDevice::PresentIdx] != queueFamiliesIndex[RenderDevice::GraphicsIdx] )
			{
				queueCreateInfos.push_back(
					{
						0u,
						queueFamiliesIndex[RenderDevice::PresentIdx],
						queuePriorities,
					} );
			}

			if ( queueFamiliesIndex[RenderDevice::ComputeIdx] != queueFamiliesIndex[RenderDevice::GraphicsIdx] )
			{
				queueCreateInfos.push_back(
					{
						0u,
						queueFamiliesIndex[RenderDevice::ComputeIdx],
						queuePriorities,
					} );
			}

			return queueCreateInfos;
		}

		ashes::DeviceCreateInfo getDeviceCreateInfo( ashes::Instance const & instance
			, ashes::Surface const & surface
			, ashes::PhysicalDevice const & gpu
			, ashes::DeviceQueueCreateInfoArray queueCreateInfos )
		{
			log::debug << "Instance enabled layers count: " << uint32_t( instance.getEnabledLayerNames().size() ) << std::endl;
			ashes::StringArray layers = instance.getEnabledLayerNames();
			ashes::StringArray extensions = { std::string{ VK_KHR_SWAPCHAIN_EXTENSION_NAME } };
			return ashes::DeviceCreateInfo
			{
				0u,
				std::move( queueCreateInfos ),
				layers,
				extensions,
				gpu.getFeatures(),
			};
		}
	}

	RenderDevice::RenderDevice( RenderSystem & renderSystem
		, ashes::PhysicalDevice const & gpu
		, AshPluginDescription const & desc
		, ashes::SurfacePtr surface )
		: renderSystem{ renderSystem }
		, gpu{ gpu }
		, desc{ desc }
		, surface{ std::move( surface ) }
		, memoryProperties{ gpu.getMemoryProperties() }
		, properties{ gpu.getProperties() }
		, features{ gpu.getFeatures() }
		, queueFamiliesIndex{ initialiseQueueFamilies( renderSystem.getInstance(), *this->surface, gpu ) }
		, device{ renderSystem.getInstance().createDevice( gpu
			, getDeviceCreateInfo( renderSystem.getInstance()
				, *this->surface
				, gpu
				, getQueueCreateInfos( queueFamiliesIndex ) ) ) }
	{
		commandPools.push_back( device->createCommandPool( queueFamiliesIndex[RenderDevice::PresentIdx]
			, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT ) );
		presentCommandPool = commandPools.back().get();
		graphicsCommandPool = presentCommandPool;
		computeCommandPool = presentCommandPool;
		transferCommandPool = presentCommandPool;
		presentQueue = device->getQueue( queueFamiliesIndex[RenderDevice::PresentIdx], 0u );
		graphicsQueue = device->getQueue( queueFamiliesIndex[RenderDevice::PresentIdx], 0u );
		computeQueue = device->getQueue( queueFamiliesIndex[RenderDevice::PresentIdx], 0u );
		transferQueue = device->getQueue( queueFamiliesIndex[RenderDevice::PresentIdx], 0u );

		if ( queueFamiliesIndex[RenderDevice::GraphicsIdx] != queueFamiliesIndex[RenderDevice::PresentIdx] )
		{
			commandPools.push_back( device->createCommandPool( queueFamiliesIndex[RenderDevice::GraphicsIdx]
				, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT ) );
			graphicsCommandPool = commandPools.back().get();
			graphicsQueue = device->getQueue( queueFamiliesIndex[RenderDevice::GraphicsIdx], 0u );
		}

		if ( queueFamiliesIndex[RenderDevice::ComputeIdx] != queueFamiliesIndex[RenderDevice::PresentIdx] )
		{
			commandPools.push_back( device->createCommandPool( queueFamiliesIndex[RenderDevice::ComputeIdx]
				, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT ) );
			computeCommandPool = commandPools.back().get();
			computeQueue = device->getQueue( queueFamiliesIndex[RenderDevice::ComputeIdx], 0u );
		}

		if ( queueFamiliesIndex[RenderDevice::TransferIdx] != queueFamiliesIndex[RenderDevice::PresentIdx] )
		{
			commandPools.push_back( device->createCommandPool( queueFamiliesIndex[RenderDevice::TransferIdx]
				, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT ) );
			transferCommandPool = commandPools.back().get();
			transferQueue = device->getQueue( queueFamiliesIndex[RenderDevice::TransferIdx], 0u );
		}

		transferCommandPool = graphicsCommandPool;
		transferQueue = device->getQueue( queueFamiliesIndex[RenderDevice::GraphicsIdx], 0u );

		bufferPool = std::make_shared< GpuBufferPool >( renderSystem, *this, cuT( "GlobalBufferPool" ) );
		uboPools = std::make_shared< UniformBufferPools >( renderSystem, *this );
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
}
