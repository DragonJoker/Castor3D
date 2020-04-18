#include "Castor3D/Render/RenderDevice.hpp"

#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Core/Instance.hpp>

namespace castor3d
{
	namespace
	{
		void initialiseQueueFamilies( ashes::Instance const & instance
			, ashes::Surface const & surface
			, ashes::PhysicalDevice const & gpu
			, uint32_t & presentQueueFamilyIndex
			, uint32_t & graphicsQueueFamilyIndex
			, uint32_t & computeQueueFamilyIndex
			, uint32_t & transferQueueFamilyIndex )
		{
			// Parcours des propriétés des files, pour vérifier leur support de la présentation.
			auto queueProps = gpu.getQueueFamilyProperties();
			std::vector< uint32_t > supportsPresent( static_cast< uint32_t >( queueProps.size() ) );
			uint32_t i{ 0u };
			graphicsQueueFamilyIndex = std::numeric_limits< uint32_t >::max();
			presentQueueFamilyIndex = std::numeric_limits< uint32_t >::max();
			computeQueueFamilyIndex = std::numeric_limits< uint32_t >::max();
			transferQueueFamilyIndex = std::numeric_limits< uint32_t >::max();

			for ( auto & present : supportsPresent )
			{
				present = surface.getSupport( i );

				if ( queueProps[i].queueCount > 0 )
				{
					if ( ashes::checkFlag( queueProps[i].queueFlags, VK_QUEUE_GRAPHICS_BIT ) )
					{
						// Tout d'abord on choisit une file graphique
						if ( graphicsQueueFamilyIndex == std::numeric_limits< uint32_t >::max() )
						{
							graphicsQueueFamilyIndex = i;
							computeQueueFamilyIndex = i;
							transferQueueFamilyIndex = i;
						}

						// Si une file supporte les graphismes et la présentation, on la préfère.
						if ( present )
						{
							graphicsQueueFamilyIndex = i;
							presentQueueFamilyIndex = i;
							break;
						}
					}
					else if ( ashes::checkFlag( queueProps[i].queueFlags, VK_QUEUE_COMPUTE_BIT )
						&& computeQueueFamilyIndex == std::numeric_limits< uint32_t >::max() )
					{
						computeQueueFamilyIndex = i;
					}
					else if ( ashes::checkFlag( queueProps[i].queueFlags, VK_QUEUE_TRANSFER_BIT )
						&& transferQueueFamilyIndex == std::numeric_limits< uint32_t >::max() )
					{
						transferQueueFamilyIndex = i;
					}
				}

				++i;
			}

			if ( presentQueueFamilyIndex == std::numeric_limits< uint32_t >::max() )
			{
				// Pas de file supportant les deux, on a donc 2 files distinctes.
				for ( size_t i = 0; i < queueProps.size(); ++i )
				{
					if ( supportsPresent[i] )
					{
						presentQueueFamilyIndex = static_cast< uint32_t >( i );
						break;
					}
				}
			}

			// Si on n'en a pas trouvé, on génère une erreur.
			if ( graphicsQueueFamilyIndex == std::numeric_limits< uint32_t >::max()
				|| presentQueueFamilyIndex == std::numeric_limits< uint32_t >::max()
				|| computeQueueFamilyIndex == std::numeric_limits< uint32_t >::max()
				|| transferQueueFamilyIndex == std::numeric_limits< uint32_t >::max() )
			{
				throw ashes::Exception{ VK_ERROR_INITIALIZATION_FAILED
					, "Queue families retrieval" };
			}
		}

		ashes::DeviceCreateInfo getDeviceCreateInfo( ashes::Instance const & instance
			, ashes::Surface const & surface
			, ashes::PhysicalDevice const & gpu
			, uint32_t & presentQueueFamilyIndex
			, uint32_t & graphicsQueueFamilyIndex
			, uint32_t & computeQueueFamilyIndex
			, uint32_t & transferQueueFamilyIndex )
		{
			initialiseQueueFamilies( instance
				, surface
				, gpu
				, presentQueueFamilyIndex
				, graphicsQueueFamilyIndex
				, computeQueueFamilyIndex
				, transferQueueFamilyIndex );
			std::vector< float > queuePriorities = { 1.0f };
			ashes::DeviceQueueCreateInfoArray queueCreateInfos;

			if ( graphicsQueueFamilyIndex != uint32_t( InvalidIndex ) )
			{
				queueCreateInfos.push_back(
					{
						0u,
						graphicsQueueFamilyIndex,
						queuePriorities,
					} );
			}

			if ( presentQueueFamilyIndex != graphicsQueueFamilyIndex )
			{
				queueCreateInfos.push_back(
					{
						0u,
						presentQueueFamilyIndex,
						queuePriorities,
					} );
			}

			if ( computeQueueFamilyIndex != graphicsQueueFamilyIndex )
			{
				queueCreateInfos.push_back(
					{
						0u,
						computeQueueFamilyIndex,
						queuePriorities,
					} );
			}

			return ashes::DeviceCreateInfo
			{
				0u,
				std::move( queueCreateInfos ),
				instance.getEnabledLayerNames(),
				{ VK_KHR_SWAPCHAIN_EXTENSION_NAME },
				gpu.getFeatures()
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
		, device{ renderSystem.getInstance().createDevice( gpu
			, getDeviceCreateInfo( renderSystem.getInstance()
				, *this->surface
				, gpu
				, presentQueueFamilyIndex
				, graphicsQueueFamilyIndex
				, computeQueueFamilyIndex
				, transferQueueFamilyIndex ) ) }
	{
		commandPools.push_back( device->createCommandPool( presentQueueFamilyIndex
			, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT ) );
		presentCommandPool = commandPools.back().get();
		graphicsCommandPool = presentCommandPool;
		computeCommandPool = presentCommandPool;
		transferCommandPool = presentCommandPool;
		presentQueue = device->getQueue( presentQueueFamilyIndex, 0u );
		graphicsQueue = device->getQueue( presentQueueFamilyIndex, 0u );
		computeQueue = device->getQueue( presentQueueFamilyIndex, 0u );
		transferQueue = device->getQueue( presentQueueFamilyIndex, 0u );

		if ( graphicsQueueFamilyIndex != presentQueueFamilyIndex )
		{
			commandPools.push_back( device->createCommandPool( graphicsQueueFamilyIndex
				, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT ) );
			graphicsCommandPool = commandPools.back().get();
			graphicsQueue = device->getQueue( graphicsQueueFamilyIndex, 0u );
		}

		if ( computeQueueFamilyIndex != presentQueueFamilyIndex )
		{
			commandPools.push_back( device->createCommandPool( computeQueueFamilyIndex
				, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT ) );
			computeCommandPool = commandPools.back().get();
			computeQueue = device->getQueue( computeQueueFamilyIndex, 0u );
		}

		if ( transferQueueFamilyIndex != presentQueueFamilyIndex )
		{
			commandPools.push_back( device->createCommandPool( transferQueueFamilyIndex
				, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT ) );
			transferCommandPool = commandPools.back().get();
			transferQueue = device->getQueue( transferQueueFamilyIndex, 0u );
		}

		transferCommandPool = graphicsCommandPool;
		transferQueue = device->getQueue( graphicsQueueFamilyIndex, 0u );
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
