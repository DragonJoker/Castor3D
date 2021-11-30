#include "Castor3D/Render/RenderDevice.hpp"

#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"

#include <CastorUtils/Miscellaneous/Debug.hpp>

#include <ashespp/Core/Instance.hpp>

#include <RenderGraph/GraphContext.hpp>

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

CU_ImplementCUSmartPtr( castor3d, RenderDevice )

namespace castor3d
{
	namespace
	{
		QueueFamilies initialiseQueueFamilies( ashes::Instance const & instance
			, ashes::PhysicalDevice const & gpu )
		{
			QueueFamilies result;
			auto queueProps = gpu.getQueueFamilyProperties();
			bool hasGraphics = false;
			uint32_t index{};

			for ( auto & queueProp : queueProps )
			{
				if ( queueProp.queueCount > 0 )
				{
					auto & data = result.emplace_back();
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

					data.setQueueSize( queueProp.queueCount );
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

		ashes::DeviceQueueCreateInfoArray getQueueCreateInfos( QueueFamilies const & queues )
		{
			ashes::DeviceQueueCreateInfoArray queueCreateInfos;

			for ( auto & queueData : queues )
			{
				std::vector< float > queuePriorities;
				queuePriorities.resize( queueData.getQueueSize(), 1.0f );

				queueCreateInfos.push_back( { 0u
					, queueData.familyIndex
					, queuePriorities } );
			}

			return queueCreateInfos;
		}

		ashes::DeviceCreateInfo getDeviceCreateInfo( ashes::Instance const & instance
			, ashes::PhysicalDevice const & gpu
			, ashes::DeviceQueueCreateInfoArray queueCreateInfos
			, ashes::StringArray const & enabledExtensions
			, VkPhysicalDeviceFeatures2 & features2 )
		{
			log::debug << "Instance enabled layers count: " << uint32_t( instance.getEnabledLayerNames().size() ) << std::endl;
			auto result = ashes::DeviceCreateInfo{ 0u
				, std::move( queueCreateInfos )
				, instance.getEnabledLayerNames()
				, enabledExtensions
				, gpu.getFeatures() };
			result->pNext = &features2;
			return result;
		}

		bool isExtensionSupported( std::string const & name
			, ashes::VkExtensionPropertiesArray const & cont )
		{
			return ( cont.end() != std::find_if( cont.begin()
				, cont.end()
				, [&name]( VkExtensionProperties const & lookup )
				{
					return lookup.extensionName == name;
				} ) );
		}
	}

	//*********************************************************************************************

	QueuesData::QueuesData( QueuesData && rhs )
		: familySupport{ rhs.familySupport }
		, familyIndex{ rhs.familyIndex }
		, m_allQueuesData{ std::move( rhs.m_allQueuesData ) }
		, m_remainingQueuesData{ std::move( rhs.m_remainingQueuesData ) }
		, m_busyQueues{ std::move( rhs.m_busyQueues ) }
	{
	}

	QueuesData & QueuesData::operator=( QueuesData && rhs )
	{
		familySupport = rhs.familySupport;
		familyIndex = rhs.familyIndex;
		m_allQueuesData = std::move( rhs.m_allQueuesData );
		m_remainingQueuesData = std::move( rhs.m_remainingQueuesData );
		m_busyQueues = std::move( rhs.m_busyQueues );
		return *this;
	}

	QueuesData::QueuesData( QueueFamilyFlags familySupport
		, uint32_t familyIndex )
		: familySupport{ familySupport }
		, familyIndex{ familyIndex }
	{
	}

	void QueuesData::initialise( ashes::Device const & device )
	{
		uint32_t index = 0u;

		for ( auto & queue : m_allQueuesData )
		{
			queue = std::make_unique< QueueData >( this );
			queue->queue = device.getQueue( familyIndex, index );
			queue->commandPool = device.createCommandPool( familyIndex
				, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT );
			m_remainingQueuesData.push_back( queue.get() );
			++index;
		}
	}

	void QueuesData::setQueueSize( size_t count )
	{
		m_allQueuesData.resize( count );
	}

	QueueData const * QueuesData::reserveQueue()const
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		auto result = m_remainingQueuesData.back();
		m_remainingQueuesData.pop_back();
		return result;
	}

	QueueData const * QueuesData::getQueue()
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		CU_Require( !m_remainingQueuesData.empty() );

		auto ires = m_busyQueues.emplace( std::this_thread::get_id()
			, QueueThreadData{} );
		auto & result = ires.first->second;

		if ( ires.second )
		{
			result.data = m_remainingQueuesData.back();
			m_remainingQueuesData.pop_back();
		}

		++result.count;
		return result.data;
	}

	void QueuesData::putQueue( QueueData const * queue )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		CU_Require( !m_busyQueues.empty() );

		auto it = m_busyQueues.find( std::this_thread::get_id() );
		CU_Require( it != m_busyQueues.end() );
		CU_Require( it->second.count > 0 );

		--it->second.count;

		if ( !it->second.count )
		{
			m_remainingQueuesData.push_back( it->second.data );
			m_busyQueues.erase( it );
		}
	}

	//*********************************************************************************************

	QueueDataWrapper::QueueDataWrapper( QueueDataWrapper && rhs )
		: parent{ rhs.parent }
		, data{ rhs.data }
	{
		rhs.parent = nullptr;
		rhs.data = nullptr;
	}

	QueueDataWrapper & QueueDataWrapper::operator=( QueueDataWrapper && rhs )
	{
		parent = rhs.parent;
		data = rhs.data;
		rhs.parent = nullptr;
		rhs.data = nullptr;
		return *this;
	}

	QueueDataWrapper::QueueDataWrapper( QueuesData * parent )
		: parent{ parent }
		, data{ parent->getQueue() }
	{
	}

	QueueDataWrapper::~QueueDataWrapper()
	{
		if ( parent && data )
		{
			parent->putQueue( data );
		}
	}

	//*********************************************************************************************

	RenderDevice::RenderDevice( RenderSystem & renderSystem
		, ashes::PhysicalDevice const & gpu
		, AshPluginDescription const & desc
		, Extensions pdeviceExtensions )
		: renderSystem{ renderSystem }
		, gpu{ gpu }
		, desc{ desc }
		, memoryProperties{ gpu.getMemoryProperties() }
		, features{ gpu.getFeatures() }
		, features12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES
			, nullptr
			, {} }
		, features11{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES
			, nullptr
			, {} }
		, drawParamsFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES
			, nullptr
		, {} }
		, features2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2
			, nullptr
			, {} }
		, properties{ gpu.getProperties() }
		, queueFamilies{ initialiseQueueFamilies( renderSystem.getInstance(), gpu ) }
		, m_deviceExtensions{ std::move( pdeviceExtensions ) }
	{
		auto apiVersion = gpu.getProperties().apiVersion;
		auto deviceExtensions = gpu.enumerateExtensionProperties( std::string{} );

		if ( apiVersion >= ashes::makeVersion( 1, 2, 0 ) )
		{
			m_deviceExtensions.addFeature( &features11 );
			m_deviceExtensions.addFeature( &features12 );
		}
		else if ( apiVersion >= ashes::makeVersion( 1, 1, 0 ) )
		{
			m_deviceExtensions.addFeature( &drawParamsFeatures );
		}
#if VK_KHR_shader_draw_parameters
		else if ( isExtensionSupported( VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME
			, deviceExtensions ) )
		{
			m_deviceExtensions.addExtension( VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME );
		}
#endif

		if ( apiVersion >= ashes::makeVersion( 1, 1, 0 ) )
		{
			// use the features2 chain to append extensions
			VkStructure * current = reinterpret_cast< VkStructure * >( &features2 );

			// build up chain of all used extension features
			for ( size_t i = 0; i < m_deviceExtensions.getSize(); i++ )
			{
				current->pNext = m_deviceExtensions[i].featureStruct;

				while ( current->pNext )
				{
					current = current->pNext;
				}
			}

			gpu.getFeatures( features2 );
			features = features2.features;

#if VK_EXT_shader_atomic_float
			if ( isExtensionSupported( VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME
				, deviceExtensions ) )
			{
				m_deviceExtensions.addExtension( VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME );
			}
#endif
		}

#if VK_KHR_synchronization2
		if ( isExtensionSupported( VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME
			, deviceExtensions ) )
		{
			m_deviceExtensions.addExtension( VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME );
		}
#endif

		m_deviceExtensions.addExtension( VK_KHR_SWAPCHAIN_EXTENSION_NAME );
		device = renderSystem.getInstance().createDevice( gpu
			, getDeviceCreateInfo( renderSystem.getInstance()
				, gpu
				, getQueueCreateInfos( queueFamilies )
				, m_deviceExtensions.getExtensionsNames()
				, features2 ) );
		device->setCallstackCallback( []()
			{
				std::stringstream callback;
				callback << castor::Debug::Backtrace{ 20, 6 };
				return callback.str();
			} );
		for ( auto & queuesData : queueFamilies )
		{
			queuesData.initialise( *device );

			auto & queueData = queuesData.front();

			if ( !m_preferredGraphicsQueue
				&& checkFlag( queuesData.familySupport, QueueFamilyFlag::eGraphics ) )
			{
				m_preferredGraphicsQueue = &queuesData;
			}

			if ( !m_preferredComputeQueue
				&& checkFlag( queuesData.familySupport, QueueFamilyFlag::eCompute ) )
			{
				m_preferredComputeQueue = &queuesData;
				computeQueue = queueData.queue.get();
				computeCommandPool = queueData.commandPool.get();
			}

			if ( !m_preferredTransferQueue
				&& checkFlag( queuesData.familySupport, QueueFamilyFlag::eTransfer ) )
			{
				m_preferredTransferQueue = &queuesData;
				transferQueue = queueData.queue.get();
				transferCommandPool = queueData.commandPool.get();
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

	QueueDataWrapper RenderDevice::graphicsData()const
	{
		return QueueDataWrapper{ m_preferredGraphicsQueue };
	}

	QueueData const * RenderDevice::reserveGraphicsData()const
	{
		return m_preferredGraphicsQueue->reserveQueue();
	}

	crg::GraphContext & RenderDevice::makeContext()const
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		auto ires = m_contexts.emplace( std::this_thread::get_id(), nullptr );
		auto it = ires.first;

		if ( ires.second )
		{
			it->second = std::make_unique< crg::GraphContext >( *device
				, VkPipelineCache{}
				, device->getAllocationCallbacks()
				, device->getMemoryProperties()
				, device->getProperties()
				, false
				, device->vkGetDeviceProcAddr );
			it->second->setCallstackCallback( []()
				{
					std::stringstream callback;
					callback << castor::Debug::Backtrace{ 20, 6 };
					return callback.str();
				} );
		}

		return *it->second;

	}
}
