#include "Castor3D/Render/RenderDevice.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/ObjectBufferPool.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"

#include <CastorUtils/Miscellaneous/Debug.hpp>

#include <ashespp/Core/Instance.hpp>

#include <RenderGraph/GraphContext.hpp>

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

CU_ImplementSmartPtr( castor3d, RenderDevice )

namespace castor3d
{
	namespace renddvc
	{
		static QueueFamilies initialiseQueueFamilies( ashes::PhysicalDevice const & gpu )
		{
			QueueFamilies result;
			auto queueProps = gpu.getQueueFamilyProperties();
			bool hasGraphics = false;
			uint32_t index{};

			for ( auto const & queueProp : queueProps )
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

		static ashes::DeviceQueueCreateInfoArray getQueueCreateInfos( QueueFamilies const & queues )
		{
			ashes::DeviceQueueCreateInfoArray queueCreateInfos;

			for ( auto & queueData : queues )
			{
				castor::Vector< float > queuePriorities;
				queuePriorities.resize( queueData.getQueueSize(), 1.0f );

				queueCreateInfos.emplace_back( 0u
					, queueData.familyIndex
					, queuePriorities );
			}

			return queueCreateInfos;
		}

		static ashes::DeviceCreateInfo getDeviceCreateInfo( ashes::Instance const & instance
			, ashes::PhysicalDevice const & gpu
			, ashes::DeviceQueueCreateInfoArray queueCreateInfos
			, ashes::StringArray const & enabledExtensions
			, VkPhysicalDeviceFeatures2 const & features2 )
		{
			log::debug << "Instance enabled layers count: " << uint32_t( instance.getEnabledLayerNames().size() ) << std::endl;
			auto result = ashes::DeviceCreateInfo{ 0u
				, castor::move( queueCreateInfos )
				, instance.getEnabledLayerNames()
				, enabledExtensions };
			result->pNext = &features2;
			return result;
		}

		static bool isExtensionSupported( castor::MbString const & name
			, ashes::VkExtensionPropertiesArray const & cont )
		{
			return ( cont.end() != std::find_if( cont.begin()
				, cont.end()
				, [&name]( VkExtensionProperties const & lookup )
				{
					return lookup.extensionName == name;
				} ) );
		}

		static bool tryAddExtension( castor::MbString const & name
			, ashes::VkExtensionPropertiesArray const & available
			, Extensions & enabled
			, void * pFeature = nullptr )
		{
			bool result = isExtensionSupported( name, available );

			if ( result )
			{
				if ( pFeature )
				{
					enabled.addExtension( name
						, reinterpret_cast< VkStructure * >( pFeature ) );
				}
				else
				{
					enabled.addExtension( name );
				}
			}

			return result;
		}
	}

	//*************************************************************************

	void Extensions::addExtension( castor::MbString const & extName )
	{
		m_extensionsNames.push_back( extName );
	}

	void Extensions::addExtension( castor::MbString const & extName
		, VkStructure * featureStruct
		, VkStructure * propertyStruct )
	{
		addExtension( extName );
		addFeature( extName, featureStruct );
		addProperty( extName, propertyStruct );
	}

	//*********************************************************************************************

	QueuesData::QueuesData( QueuesData && rhs )noexcept
		: familySupport{ rhs.familySupport }
		, familyIndex{ rhs.familyIndex }
		, m_allQueuesData{ castor::move( rhs.m_allQueuesData ) }
		, m_remainingQueuesData{ castor::move( rhs.m_remainingQueuesData ) }
		, m_busyQueues{ castor::move( rhs.m_busyQueues ) }
	{
	}

	QueuesData & QueuesData::operator=( QueuesData && rhs )noexcept
	{
		familySupport = rhs.familySupport;
		familyIndex = rhs.familyIndex;
		m_allQueuesData = castor::move( rhs.m_allQueuesData );
		m_remainingQueuesData = castor::move( rhs.m_remainingQueuesData );
		m_busyQueues = castor::move( rhs.m_busyQueues );
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
			castor::MbString name = "Queue_" + castor::string::toMbString( familyIndex ) + "_" + castor::string::toMbString( index );
			queue = castor::make_unique< QueueData >( this );
			queue->queue = device.getQueue( name, familyIndex, index );
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

	void QueuesData::unreserveQueue( QueueData const * queue )const noexcept
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		m_remainingQueuesData.push_back( queue );
	}

	QueueData const * QueuesData::getQueue()
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );

		if ( m_allQueuesData.size() > 1 )
		{
			CU_Require( !m_remainingQueuesData.empty() );
		}
		else
		{
			if ( auto it = m_busyQueues.find( std::this_thread::get_id() );
				it != m_busyQueues.end() && it->second.data )
			{
				++it->second.count;
				return it->second.data;
			}

			while ( m_remainingQueuesData.empty() )
			{
				m_mutex.unlock();
				std::this_thread::sleep_for( 1_ms );
				m_mutex.lock();
			}
		}

		auto [it, res] = m_busyQueues.try_emplace( std::this_thread::get_id() );
		auto & result = it->second;

		if ( res )
		{
			result.data = m_remainingQueuesData.back();
			m_remainingQueuesData.pop_back();
		}

#ifndef NDEBUG
		castor::StringStream stream;
		stream << castor::debug::Backtrace{};
		result.callstack = stream.str();
#endif

		++result.count;
		return result.data;
	}

	void QueuesData::putQueue( QueueData const * /*queue*/ )noexcept
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

	QueueDataWrapper::QueueDataWrapper()
		: parent{ nullptr }
		, data{ nullptr }
	{
	}

	QueueDataWrapper::QueueDataWrapper( QueueDataWrapper && rhs )noexcept
		: parent{ rhs.parent }
		, data{ rhs.data }
	{
		rhs.parent = nullptr;
		rhs.data = nullptr;
	}

	QueueDataWrapper & QueueDataWrapper::operator=( QueueDataWrapper && rhs )noexcept
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

	QueueDataWrapper::~QueueDataWrapper()noexcept
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
		, properties{ gpu.getProperties() }
		, queueFamilies{ renddvc::initialiseQueueFamilies( gpu ) }
		, m_deviceExtensions{ castor::move( pdeviceExtensions ) }
		, m_availableExtensions{ gpu.enumerateExtensionProperties( castor::MbString{} ) }
	{
		auto apiVersion = gpu.getProperties().apiVersion;
		bool hasFeatures11 = false;
		bool hasFeatures2 = false;
		bool hasVulkan1_1 = false;
		bool hasFloatControls = false;
		bool hasSpirv1_4 = false;
		bool hasDescriptorIndexing = false;
		bool hasDeviceAddress = false;

#if VK_VERSION_1_3
		if ( apiVersion >= ashes::makeVersion( 1, 3, 0 ) )
		{
			m_hasFeatures13 = true;
			hasDescriptorIndexing = true;
			hasDeviceAddress = true;
			m_deviceExtensions.addFeature( &m_features13 );
			m_deviceExtensions.addProperty( &m_properties13 );
		}
#endif
#if VK_VERSION_1_2
		if ( apiVersion >= ashes::makeVersion( 1, 2, 0 ) )
		{
			hasFeatures2 = true;
			hasFeatures11 = true;
			m_hasFeatures12 = true;
			hasVulkan1_1 = true;
			hasFloatControls = true;
			hasSpirv1_4 = true;
			hasDescriptorIndexing = true;
			hasDeviceAddress = true;
			m_deviceExtensions.addFeature( &m_features11 );
			m_deviceExtensions.addProperty( &m_properties11 );
			m_deviceExtensions.addFeature( &m_features12 );
			m_deviceExtensions.addProperty( &m_properties12 );
		}
		else
		{
#endif
			if ( apiVersion >= ashes::makeVersion( 1, 1, 0 ) )
			{
				hasFeatures2 = true;
				hasVulkan1_1 = true;
				m_deviceExtensions.addFeature( &m_drawParamsFeatures );
			}
#if VK_KHR_shader_draw_parameters
			else
			{
				doTryAddExtension( VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME );
			}
#endif
#if VK_VERSION_1_2
		}
#endif
#if VK_KHR_get_physical_device_properties2
		if ( !hasFeatures2 )
		{
			hasFeatures2 = doTryAddExtension( VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME );
		}
#endif
		if ( hasFeatures2 )
		{
#if defined( VK_KHR_portability_subset )
			doTryAddExtension( VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME, &m_portabilitySubsetFeatures, &m_portabilitySubsetProperties );
#endif
#if VK_KHR_shader_float_controls
			if ( !hasFloatControls )
			{
				hasFloatControls = doTryAddExtension( VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME );
			}
#endif
#if VK_EXT_device_fault
			doTryAddExtension( VK_EXT_DEVICE_FAULT_EXTENSION_NAME );
#endif
#if VK_KHR_spirv_1_4
			if ( !hasSpirv1_4 && hasVulkan1_1 && hasFloatControls )
			{
				hasSpirv1_4 = doTryAddExtension( VK_KHR_SPIRV_1_4_EXTENSION_NAME );
			}
#endif
#if VK_EXT_shader_atomic_float
			doTryAddExtension( VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME, &m_atomicFloatAddFeatures );
#endif
#if C3D_UseMeshShaders
#	if defined( VK_NV_mesh_shader ) || defined( VK_EXT_mesh_shader )
#		if defined( VK_NV_mesh_shader ) && defined( VK_EXT_mesh_shader )
			if ( prefersMeshShaderEXT() )
			{
				if ( !doTryAddExtension( VK_EXT_MESH_SHADER_EXTENSION_NAME, &m_meshShaderFeaturesEXT, &m_meshShaderPropertiesEXT ) )
				{
					m_prefersMeshShaderEXT = false;
					doTryAddExtension( VK_NV_MESH_SHADER_EXTENSION_NAME, &m_meshShaderFeaturesNV, &m_meshShaderPropertiesNV );
				}
			}
			else
			{
				if ( !doTryAddExtension( VK_NV_MESH_SHADER_EXTENSION_NAME, &m_meshShaderFeaturesNV, &m_meshShaderPropertiesNV ) )
				{
					m_prefersMeshShaderEXT = true;
					doTryAddExtension( VK_EXT_MESH_SHADER_EXTENSION_NAME, &m_meshShaderFeaturesEXT, &m_meshShaderPropertiesEXT );
				}
			}
#		elif defined( VK_EXT_mesh_shader )
			m_prefersMeshShaderEXT = true;
			doTryAddExtension( VK_EXT_MESH_SHADER_EXTENSION_NAME, &m_meshShaderFeaturesEXT, &m_meshShaderPropertiesEXT );
#		else
			m_prefersMeshShaderEXT = false;
			doTryAddExtension( VK_NV_MESH_SHADER_EXTENSION_NAME, &m_meshShaderFeaturesNV, &m_meshShaderPropertiesNV );
#		endif
#	endif
#endif
#if VK_NV_compute_shader_derivatives
			doTryAddExtension( VK_NV_COMPUTE_SHADER_DERIVATIVES_EXTENSION_NAME, &m_computeDerivativesFeatures, nullptr );
#endif

			if ( !m_hasFeatures13 )
			{
#if VK_KHR_shader_terminate_invocation
				doTryAddExtension( VK_KHR_SHADER_TERMINATE_INVOCATION_EXTENSION_NAME, &m_terminateInvocationFeatures );
#endif
#if VK_EXT_shader_demote_to_helper_invocation
				doTryAddExtension( VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME, &m_demoteToHelperInvocationFeatures );
#endif
			}

			if ( !m_hasFeatures12 )
			{
#if VK_EXT_descriptor_indexing
				hasDescriptorIndexing = doTryAddExtension( VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME, &m_descriptorIndexingFeatures, &m_descriptorIndexingProperties );
#endif
#if VK_KHR_buffer_device_address
				hasDeviceAddress = doTryAddExtension( VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME, &m_bufferDeviceAddressFeatures );
#endif
#if VK_KHR_8bit_storage
				doTryAddExtension( VK_KHR_8BIT_STORAGE_EXTENSION_NAME, &m_8bitFeatures );
#endif
#if VK_KHR_shader_float16_int8
				doTryAddExtension( VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME, &m_f16i8bitFeatures );
#endif
			}

#if VK_EXT_shader_subgroup_ballot
			doTryAddExtension( VK_EXT_SHADER_SUBGROUP_BALLOT_EXTENSION_NAME );
#endif
#if VK_KHR_acceleration_structure
			if ( hasDescriptorIndexing
				&& hasDeviceAddress
				&& doTryAddExtension( VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, &m_accelFeatures, &m_accelProperties )
				&& hasSpirv1_4 )
			{
#	if VK_KHR_ray_tracing_pipeline
				doTryAddExtension( VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME, &m_rtPipelineFeatures, &m_rtPipelineProperties );
#	endif
			}
#endif

			if ( !hasFeatures11 )
			{
#if VK_KHR_16bit_storage
				doTryAddExtension( VK_KHR_16BIT_STORAGE_EXTENSION_NAME, &m_16bitFeatures );
#endif
			}
#if VK_KHR_synchronization2
			doTryAddExtension( VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME );
#endif
			// use the features2 chain to append extensions
			auto currentFeat = reinterpret_cast< VkStructure * >( &m_features2 );

			// build up chain of all used extension features
			for ( auto & feats : m_deviceExtensions.getFeatures() )
			{
				currentFeat->pNext = feats.extStruct;
				currentFeat = currentFeat->pNext;
			}

			auto currentProp = reinterpret_cast< VkStructure * >( &m_properties2 );

			// build up chain of all used extension properties
			for ( auto & props : m_deviceExtensions.getProperties() )
			{
				currentProp->pNext = props.extStruct;
				currentProp = currentProp->pNext;
			}

			gpu.getFeatures( m_features2 );
			gpu.getProperties( m_properties2 );
			features = m_features2.features;
		}
#if VK_KHR_deferred_host_operations
		doTryAddExtension( VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME );
#endif

		if ( !doTryAddExtension( VK_KHR_SWAPCHAIN_EXTENSION_NAME ) )
		{
			CU_Exception( "Swapchain extension not supported." );
		}

		device = renderSystem.getInstance().createDevice( gpu
			, renddvc::getDeviceCreateInfo( renderSystem.getInstance()
				, gpu
				, renddvc::getQueueCreateInfos( queueFamilies )
				, m_deviceExtensions.getExtensionsNames()
				, m_features2 ) );
		device->setCallstackCallback( []()
			{
				castor::StringStream callback;
				callback << castor::debug::Backtrace{ 20, 6 };
				return castor::toUtf8( callback.str() );
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

		bufferPool = castor::makeUnique< GpuBufferPool >( *this, cuT( "GlobalBufferPool" ) );
		geometryPools = castor::makeUnique< ObjectBufferPool >( *this, cuT( "ModelBuffersPool" ) );
		vertexPools = castor::makeUnique< VertexBufferPool >( *this, cuT( "VertexBuffersPool" ) );
		indexPools = castor::makeUnique< IndexBufferPool >( *this, cuT( "IndexBuffersPool" ) );
		uboPool = castor::makeUnique< UniformBufferPool >( *this, cuT( "UniformBufferPool" ) );
	}

	RenderDevice::~RenderDevice()noexcept
	{
		{
			auto lock = castor::makeUniqueLock( m_mutex );
			m_contexts.clear();
		}
		uboPool.reset();
		uboPool.reset();
		indexPools.reset();
		vertexPools.reset();
		geometryPools.reset();
		bufferPool.reset();
		queueFamilies.clear();
		device.reset();
	}

	VkFormat RenderDevice::selectSuitableDepthFormat( VkFormatFeatureFlags requiredFeatures )const
	{
		castor::Vector< VkFormat > depthFormats
		{
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_X8_D24_UNORM_PACK32,
			VK_FORMAT_D16_UNORM,
		};
		return selectSuitableFormat( depthFormats, requiredFeatures );
	}

	VkFormat RenderDevice::selectSuitableStencilFormat( VkFormatFeatureFlags requiredFeatures )const
	{
		castor::Vector< VkFormat > depthFormats
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
		castor::Vector< VkFormat > depthFormats
		{
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
		};
		return selectSuitableFormat( depthFormats, requiredFeatures );
	}

	VkFormat RenderDevice::selectSmallestFormatRSFloatFormat( VkFormatFeatureFlags requiredFeatures )const
	{
		castor::Vector< VkFormat > formats
		{
			VK_FORMAT_R16_SFLOAT,
			VK_FORMAT_R32_SFLOAT,
		};
		return selectSuitableFormat( formats, requiredFeatures );
	}

	VkFormat RenderDevice::selectSmallestFormatRGSFloatFormat( VkFormatFeatureFlags requiredFeatures )const
	{
		castor::Vector< VkFormat > formats
		{
			VK_FORMAT_R16G16_SFLOAT,
			VK_FORMAT_R32G32_SFLOAT,
			VK_FORMAT_R16G16B16A16_SFLOAT,
			VK_FORMAT_R32G32B32A32_SFLOAT,
		};
		return selectSuitableFormat( formats, requiredFeatures );
	}

	VkFormat RenderDevice::selectSmallestFormatRGBUFloatFormat( VkFormatFeatureFlags requiredFeatures )const
	{
		castor::Vector< VkFormat > formats
		{
			VK_FORMAT_B10G11R11_UFLOAT_PACK32,
			VK_FORMAT_R16G16B16_SFLOAT,
			VK_FORMAT_R32G32B32_SFLOAT,
			VK_FORMAT_R16G16B16A16_SFLOAT,
			VK_FORMAT_R32G32B32A32_SFLOAT,
		};
		return selectSuitableFormat( formats, requiredFeatures );
	}

	VkFormat RenderDevice::selectSmallestFormatRGBSFloatFormat( VkFormatFeatureFlags requiredFeatures )const
	{
		castor::Vector< VkFormat > formats
		{
			VK_FORMAT_R16G16B16_SFLOAT,
			VK_FORMAT_R32G32B32_SFLOAT,
			VK_FORMAT_R16G16B16A16_SFLOAT,
			VK_FORMAT_R32G32B32A32_SFLOAT,
		};
		return selectSuitableFormat( formats, requiredFeatures );
	}

	VkFormat RenderDevice::selectSuitableFormat( castor::Vector< VkFormat > const & formats
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

		if ( it != formats.begin() )
		{
			log::warn << "The first format (" << castor::makeString( ashes::getName( *formats.begin() ) )
				<< ") did not support the wanted properties, using a secondary choice (" << castor::makeString( ashes::getName( *it ) )
				<< ").\n";
		}

		return *it;
	}

	QueueDataWrapper RenderDevice::graphicsData()const noexcept
	{
		return QueueDataWrapper{ m_preferredGraphicsQueue };
	}

	size_t RenderDevice::graphicsQueueSize()const noexcept
	{
		return m_preferredGraphicsQueue->getQueueSize();
	}

	QueueData const * RenderDevice::reserveGraphicsData()const noexcept
	{
		return m_preferredGraphicsQueue->reserveQueue();
	}

	void RenderDevice::unreserveGraphicsData( QueueData const * queueData )const noexcept
	{
		m_preferredGraphicsQueue->unreserveQueue( queueData );
	}

	void RenderDevice::putGraphicsData( QueueData const * queueData )const noexcept
	{
		m_preferredGraphicsQueue->putQueue( queueData );
	}

	crg::GraphContext & RenderDevice::makeContext()const noexcept
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		auto [it, res] = m_contexts.try_emplace( std::this_thread::get_id() );

		if ( res )
		{
			it->second = castor::make_unique< crg::GraphContext >( *device
				, VkPipelineCache{}
				, device->getAllocationCallbacks()
				, device->getMemoryProperties()
				, device->getProperties()
				, false
				, device->vkGetDeviceProcAddr );
			it->second->setCallstackCallback( []()
				{
					castor::StringStream callback;
					callback << castor::debug::Backtrace{ 20, 6 };
					return castor::toUtf8( callback.str() );
				} );
		}

		return *it->second;
	}

	bool RenderDevice::hasExtension( castor::MbStringView name )const noexcept
	{
		auto it = std::find_if( m_deviceExtensions.getExtensionsNames().begin()
			, m_deviceExtensions.getExtensionsNames().end()
			, [&name]( castor::MbString const & lookup )
			{
				return lookup == name;
			} );
		return it != m_deviceExtensions.getExtensionsNames().end();
	}

	bool RenderDevice::hasTerminateInvocation()const noexcept
	{
#if VK_VERSION_1_3
		if ( m_hasFeatures13 )
		{
			return m_features13.shaderTerminateInvocation == VK_TRUE;
		}
#endif
#if VK_KHR_shader_terminate_invocation
		return m_terminateInvocationFeatures.shaderTerminateInvocation == VK_TRUE;
#else
		return false;
#endif
	}

	bool RenderDevice::hasDemoteToHelperInvocation()const noexcept
	{
#if VK_VERSION_1_3
		if ( m_hasFeatures13 )
		{
			return m_features13.shaderDemoteToHelperInvocation == VK_TRUE;
		}
#endif
#if VK_EXT_shader_demote_to_helper_invocation
		return m_demoteToHelperInvocationFeatures.shaderDemoteToHelperInvocation == VK_TRUE;
#else
		return false;
#endif
	}

	bool RenderDevice::hasMeshAndTaskShaders()const noexcept
	{
		return hasMeshShaders() && hasTaskShaders();
	}

	bool RenderDevice::hasMeshShaders()const noexcept
	{
#if C3D_UseMeshShaders
#	if VK_EXT_mesh_shader || VK_NV_mesh_shader
#		if VK_EXT_mesh_shader && VK_NV_mesh_shader
		if ( prefersMeshShaderEXT() )
		{
			return m_meshShaderFeaturesEXT.meshShader == VK_TRUE;
		}

		return m_meshShaderFeaturesNV.meshShader == VK_TRUE;
#		elif VK_EXT_mesh_shader
		return m_meshShaderFeaturesEXT.meshShader == VK_TRUE;
#		else
		return m_meshShaderFeaturesNV.meshShader == VK_TRUE;
#		endif
#	else
		return false;
#	endif
#else
		return false;
#endif
	}

	bool RenderDevice::hasTaskShaders()const noexcept
	{
#if C3D_UseMeshShaders && C3D_UseTaskShaders
#	if VK_EXT_mesh_shader || VK_NV_mesh_shader
#		if VK_EXT_mesh_shader && VK_NV_mesh_shader
		if ( prefersMeshShaderEXT() )
		{
			return m_meshShaderFeaturesEXT.taskShader == VK_TRUE;
		}

		return m_meshShaderFeaturesNV.taskShader == VK_TRUE;
#		elif VK_EXT_mesh_shader
		return m_meshShaderFeaturesEXT.taskShader == VK_TRUE;
#		else
		return m_meshShaderFeaturesNV.taskShader == VK_TRUE;
#		endif
#	else
		return false;
#	endif
#else
		return false;
#endif
	}

	bool RenderDevice::hasAtomicFloatAdd()const noexcept
	{
#if VK_EXT_shader_atomic_float
		return m_atomicFloatAddFeatures.shaderBufferFloat32AtomicAdd == VK_TRUE
			&& m_atomicFloatAddFeatures.shaderBufferFloat64AtomicAdd == VK_TRUE;
#else
		return false;
#endif
	}

	bool RenderDevice::hasBufferDeviceAddress()const noexcept
	{
#if VK_VERSION_1_2
		if ( m_hasFeatures12 )
		{
			return m_features12.bufferDeviceAddress == VK_TRUE;
		}
#endif
#if VK_EXT_buffer_device_address
		return m_bufferDeviceAddressFeatures.bufferDeviceAddress == VK_TRUE;
#else
		return false;
#endif
	}

	bool RenderDevice::hasRayTracing()const noexcept
	{
#if VK_KHR_acceleration_structure && VK_KHR_ray_tracing_pipeline
		return m_accelFeatures.accelerationStructure == VK_TRUE
			&& m_rtPipelineFeatures.rayTracingPipeline == VK_TRUE;
#else
		return false;
#endif
	}

	uint32_t RenderDevice::getMaxBindlessSampled()const noexcept
	{
#if VK_VERSION_1_2
		if ( m_hasFeatures12 )
		{
			return std::min( MaxBindlessResources, m_properties12.maxDescriptorSetUpdateAfterBindSampledImages );
		}
#endif
#if VK_EXT_descriptor_indexing
		return std::min( MaxBindlessResources, m_descriptorIndexingProperties.maxDescriptorSetUpdateAfterBindSampledImages );
#else
		return 0u;
#endif
	}

	uint32_t RenderDevice::getMaxBindlessStorage()const noexcept
	{
#if VK_VERSION_1_2
		if ( m_hasFeatures12 )
		{
			return std::min( MaxBindlessResources, m_properties12.maxDescriptorSetUpdateAfterBindStorageBuffers );
		}
#endif
#if VK_EXT_descriptor_indexing
		return std::min( MaxBindlessResources, m_descriptorIndexingProperties.maxDescriptorSetUpdateAfterBindStorageBuffers );
#else
		return 0u;
#endif
	}

	bool RenderDevice::hasBindless()const noexcept
	{
#if VK_VERSION_1_2
		if ( m_hasFeatures12 )
		{
			return m_features12.descriptorBindingPartiallyBound == VK_TRUE
				&& m_features12.descriptorBindingSampledImageUpdateAfterBind == VK_TRUE
				&& m_features12.runtimeDescriptorArray == VK_TRUE;
		}
#endif
#if VK_EXT_descriptor_indexing
		return m_descriptorIndexingFeatures.descriptorBindingPartiallyBound == VK_TRUE
			&& m_descriptorIndexingFeatures.descriptorBindingSampledImageUpdateAfterBind == VK_TRUE
			&& m_descriptorIndexingFeatures.runtimeDescriptorArray == VK_TRUE;
#else
		return false;
#endif
	}

	void RenderDevice::fillGPUMeshInformations( GpuInformations & gpuInformations )const noexcept
	{
#if VK_EXT_mesh_shader || VK_NV_mesh_shader
#	if VK_EXT_mesh_shader && VK_NV_mesh_shader
		if ( prefersMeshShaderEXT() )
		{
			if ( hasMeshShaders() )
			{
				auto & meshLimits = getMeshPropertiesEXT();
				gpuInformations.setValue( GpuMax::eMeshWorkGroupInvocations, meshLimits.maxMeshWorkGroupInvocations );
				gpuInformations.setValue( GpuMax::eMeshWorkGroupSizeX, meshLimits.maxMeshWorkGroupSize[0] );
				gpuInformations.setValue( GpuMax::eMeshWorkGroupSizeY, meshLimits.maxMeshWorkGroupSize[1] );
				gpuInformations.setValue( GpuMax::eMeshWorkGroupSizeZ, meshLimits.maxMeshWorkGroupSize[2] );
				gpuInformations.setValue( GpuMax::eMeshOutputVertices, meshLimits.maxMeshOutputVertices );
				gpuInformations.setValue( GpuMax::eMeshOutputPrimitives, meshLimits.maxMeshOutputPrimitives );
			}

			if ( hasTaskShaders() )
			{
				auto & meshLimits = getMeshPropertiesEXT();
				gpuInformations.setValue( GpuMax::eTaskWorkGroupInvocations, meshLimits.maxTaskWorkGroupInvocations );
				gpuInformations.setValue( GpuMax::eTaskWorkGroupSizeX, meshLimits.maxTaskWorkGroupSize[0] );
				gpuInformations.setValue( GpuMax::eTaskWorkGroupSizeY, meshLimits.maxTaskWorkGroupSize[1] );
				gpuInformations.setValue( GpuMax::eTaskWorkGroupSizeZ, meshLimits.maxTaskWorkGroupSize[2] );
			}
		}
		else
		{
			if ( hasMeshShaders() )
			{
				auto & meshLimits = getMeshPropertiesNV();
				gpuInformations.setValue( GpuMax::eMeshWorkGroupInvocations, meshLimits.maxMeshWorkGroupInvocations );
				gpuInformations.setValue( GpuMax::eMeshWorkGroupSizeX, meshLimits.maxMeshWorkGroupSize[0] );
				gpuInformations.setValue( GpuMax::eMeshWorkGroupSizeY, meshLimits.maxMeshWorkGroupSize[1] );
				gpuInformations.setValue( GpuMax::eMeshWorkGroupSizeZ, meshLimits.maxMeshWorkGroupSize[2] );
				gpuInformations.setValue( GpuMax::eMeshOutputVertices, meshLimits.maxMeshOutputVertices );
				gpuInformations.setValue( GpuMax::eMeshOutputPrimitives, meshLimits.maxMeshOutputPrimitives );
			}

			if ( hasTaskShaders() )
			{
				auto & meshLimits = getMeshPropertiesNV();
				gpuInformations.setValue( GpuMax::eTaskWorkGroupInvocations, meshLimits.maxTaskWorkGroupInvocations );
				gpuInformations.setValue( GpuMax::eTaskWorkGroupSizeX, meshLimits.maxTaskWorkGroupSize[0] );
				gpuInformations.setValue( GpuMax::eTaskWorkGroupSizeY, meshLimits.maxTaskWorkGroupSize[1] );
				gpuInformations.setValue( GpuMax::eTaskWorkGroupSizeZ, meshLimits.maxTaskWorkGroupSize[2] );
			}
		}
#	elif VK_EXT_mesh_shader
		if ( hasMeshShaders() )
		{
			auto & meshLimits = getMeshPropertiesEXT();
			gpuInformations.setValue( GpuMax::eMeshWorkGroupInvocations, meshLimits.maxMeshWorkGroupInvocations );
			gpuInformations.setValue( GpuMax::eMeshWorkGroupSizeX, meshLimits.maxMeshWorkGroupSize[0] );
			gpuInformations.setValue( GpuMax::eMeshWorkGroupSizeY, meshLimits.maxMeshWorkGroupSize[1] );
			gpuInformations.setValue( GpuMax::eMeshWorkGroupSizeZ, meshLimits.maxMeshWorkGroupSize[2] );
			gpuInformations.setValue( GpuMax::eMeshOutputVertices, meshLimits.maxMeshOutputVertices );
			gpuInformations.setValue( GpuMax::eMeshOutputPrimitives, meshLimits.maxMeshOutputPrimitives );
		}

		if ( hasTaskShaders() )
		{
			auto & meshLimits = getMeshPropertiesEXT();
			gpuInformations.setValue( GpuMax::eTaskWorkGroupInvocations, meshLimits.maxTaskWorkGroupInvocations );
			gpuInformations.setValue( GpuMax::eTaskWorkGroupSizeX, meshLimits.maxTaskWorkGroupSize[0] );
			gpuInformations.setValue( GpuMax::eTaskWorkGroupSizeY, meshLimits.maxTaskWorkGroupSize[1] );
			gpuInformations.setValue( GpuMax::eTaskWorkGroupSizeZ, meshLimits.maxTaskWorkGroupSize[2] );
		}
#	else
		if ( hasMeshShaders() )
		{
			auto & meshLimits = getMeshPropertiesNV();
			gpuInformations.setValue( GpuMax::eMeshWorkGroupInvocations, meshLimits.maxMeshWorkGroupInvocations );
			gpuInformations.setValue( GpuMax::eMeshWorkGroupSizeX, meshLimits.maxMeshWorkGroupSize[0] );
			gpuInformations.setValue( GpuMax::eMeshWorkGroupSizeY, meshLimits.maxMeshWorkGroupSize[1] );
			gpuInformations.setValue( GpuMax::eMeshWorkGroupSizeZ, meshLimits.maxMeshWorkGroupSize[2] );
			gpuInformations.setValue( GpuMax::eMeshOutputVertices, meshLimits.maxMeshOutputVertices );
			gpuInformations.setValue( GpuMax::eMeshOutputPrimitives, meshLimits.maxMeshOutputPrimitives );
		}

		if ( hasTaskShaders() )
		{
			auto & meshLimits = m_device->getMeshPropertiesNV();
			gpuInformations.setValue( GpuMax::eTaskWorkGroupInvocations, meshLimits.maxTaskWorkGroupInvocations );
			gpuInformations.setValue( GpuMax::eTaskWorkGroupSizeX, meshLimits.maxTaskWorkGroupSize[0] );
			gpuInformations.setValue( GpuMax::eTaskWorkGroupSizeY, meshLimits.maxTaskWorkGroupSize[1] );
			gpuInformations.setValue( GpuMax::eTaskWorkGroupSizeZ, meshLimits.maxTaskWorkGroupSize[2] );
		}
#	endif
#endif
	}

	bool RenderDevice::hasGeometryShader()const noexcept
	{
		return features.geometryShader == VK_TRUE;
	}

	bool RenderDevice::hasDrawId()const noexcept
	{
#if VK_VERSION_1_1
			return m_features11.shaderDrawParameters == VK_TRUE;
#elif VK_KHR_draw_parameters
			return m_drawParamsFeatures.shaderDrawParameters == VK_TRUE;
#endif
	}

	bool RenderDevice::doTryAddExtension( castor::MbString const & name
		, void * pFeature
		, void * pProperty )
	{
		bool result = renddvc::tryAddExtension( name, m_availableExtensions, m_deviceExtensions, pFeature );

		if ( result  && pProperty )
		{
			m_deviceExtensions.addProperty( name, pProperty );
		}

		return result;
	}
}
