/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderDevice_H___
#define ___C3D_RenderDevice_H___

#include "RenderModule.hpp"
#include "Castor3D/Buffer/BufferModule.hpp"

#include <CastorUtils/Design/FlagCombination.hpp>

#include <ashespp/Command/CommandPool.hpp>
#include <ashespp/Sync/Queue.hpp>

#include <RenderGraph/FrameGraphPrerequisites.hpp>
#include <RenderGraph/GraphContext.hpp>

#include <array>
#include <vector>

namespace castor3d
{
	struct VkStructure
	{
	private:
		VkStructure();

	public:
		VkStructureType sType;
		VkStructure * pNext;
	};

	struct ExtensionStruct
	{
		ExtensionStruct( castor::MbString extName
			, VkStructure * extStruct )
			: extName{ castor::move( extName ) }
			, extStruct{ extStruct }
		{
		}

		castor::MbString extName;
		VkStructure * extStruct;
	};

	using FeatureArray = castor::Vector< ExtensionStruct >;
	using PropertyArray = castor::Vector< ExtensionStruct >;

	struct Extensions
	{
	public:
		/**
		*\~english
		*\brief
		*	Adds an extension.
		*\~french
		*\brief
		*	Ajoute une extension.
		*/
		C3D_API void addExtension( castor::MbString const & extName );
		/**
		*\~english
		*\brief
		*	Adds an extension, and it's optional feature structure, that will be queried on physical device.
		*\~french
		*\brief
		*	Ajoute une extension, et sa feature structure optionnelle, qui sera remplie via le physical device.
		*/
		C3D_API void addExtension( castor::MbString const & extName
			, VkStructure * featureStruct
			, VkStructure * propertyStruct = nullptr );
		/**
		*\~english
		*\brief
		*	Adds a feature structure, that will be queried on physical device.
		*\~french
		*\brief
		*	Ajoute une feature structure, qui sera remplie via le physical device.
		*/
		template< typename StructT >
		void addFeature( castor::MbString const & extName
			, StructT * featureStruct )
		{
			if ( featureStruct )
			{
				m_features.push_back( { extName
					, reinterpret_cast< VkStructure * >( featureStruct ) } );
			}
		}
		/**
		*\~english
		*\brief
		*	Adds a feature structure, that will be queried on physical device.
		*\~french
		*\brief
		*	Ajoute une feature structure, qui sera remplie via le physical device.
		*/
		template< typename StructT >
		void addFeature( StructT * featureStruct )
		{
			addFeature( castor::MbString{}, featureStruct );
		}
		/**
		*\~english
		*\brief
		*	Adds a feature structure, that will be queried on physical device.
		*\~french
		*\brief
		*	Ajoute une feature structure, qui sera remplie via le physical device.
		*/
		template< typename StructT >
		void addProperty( castor::MbString const & extName
			, StructT * propStruct )
		{
			if ( propStruct )
			{
				m_properties.push_back( { extName
					, reinterpret_cast< VkStructure * >( propStruct ) } );
			}
		}
		/**
		*\~english
		*\brief
		*	Adds a feature structure, that will be queried on physical device.
		*\~french
		*\brief
		*	Ajoute une feature structure, qui sera remplie via le physical device.
		*/
		template< typename StructT >
		void addProperty( StructT * propStruct )
		{
			addProperty( castor::MbString{}, propStruct );
		}

		ashes::StringArray const & getExtensionsNames()const noexcept
		{
			return m_extensionsNames;
		}

		FeatureArray & getFeatures()noexcept
		{
			return m_features;
		}

		FeatureArray const & getFeatures()const noexcept
		{
			return m_features;
		}

		PropertyArray & getProperties()noexcept
		{
			return m_properties;
		}

		PropertyArray const & getProperties()const noexcept
		{
			return m_properties;
		}

	private:
		ashes::StringArray m_extensionsNames;
		FeatureArray m_features;
		PropertyArray m_properties;
	};

	enum class QueueFamilyFlag
	{
		eNone = 0x00,
		ePresent = 0x01,
		eGraphics = 0x02,
		eTransfer = 0x04,
		eCompute = 0x08,
	};
	CU_ImplementFlags( QueueFamilyFlag )

	struct QueueData
	{
		QueueData( QueueData const & ) = delete;
		QueueData & operator=( QueueData const & ) = delete;
		C3D_API QueueData( QueueData && )noexcept = default;
		C3D_API QueueData & operator=( QueueData && )noexcept = default;

		explicit QueueData( QueuesData const * parent )noexcept
			: parent{ parent }
		{
		}

		QueuesData const * parent;
		ashes::QueuePtr queue;
		ashes::CommandPoolPtr commandPool;
	};
	using QueueDataPtr = castor::RawUniquePtr< QueueData >;

	struct QueueDataWrapper
	{
		QueueDataWrapper( QueueDataWrapper const & ) = delete;
		QueueDataWrapper & operator=( QueueDataWrapper const & ) = delete;

		C3D_API QueueDataWrapper();
		C3D_API QueueDataWrapper( QueueDataWrapper && rhs )noexcept;
		C3D_API QueueDataWrapper & operator=( QueueDataWrapper && rhs )noexcept;
		C3D_API explicit QueueDataWrapper( QueuesData * parent );
		C3D_API ~QueueDataWrapper()noexcept;

		QueueData const * release()noexcept
		{
			auto result = data;
			data = nullptr;
			parent = nullptr;
			return result;
		}

		QueueData const * operator->()const noexcept
		{
			return data;
		}

		QueueData const & operator*()const noexcept
		{
			return *data;
		}

		QueuesData * parent;
		QueueData const * data;
	};

	struct QueuesData
	{
		friend struct RenderDevice;

		QueuesData( QueuesData const & ) = delete;
		QueuesData & operator=( QueuesData const & ) = delete;
		C3D_API QueuesData( QueuesData && rhs )noexcept;
		C3D_API QueuesData & operator=( QueuesData && rhs )noexcept;
		C3D_API ~QueuesData()noexcept = default;

		C3D_API QueuesData( QueueFamilyFlags familySupport = QueueFamilyFlag::eNone
			, uint32_t familyIndex = 0xFFFFFFFFu );

		C3D_API void initialise( ashes::Device const & device );
		C3D_API void setQueueSize( size_t count );
		C3D_API QueueData const * reserveQueue()const;
		C3D_API void unreserveQueue( QueueData const * queue )const noexcept;
		C3D_API QueueData const * getQueue();
		C3D_API void putQueue( QueueData const * queue )noexcept;

		size_t getQueueSize()const noexcept
		{
			return m_allQueuesData.size();
		}

		auto begin()const noexcept
		{
			return m_allQueuesData.begin();
		}

		auto end()const noexcept
		{
			return m_allQueuesData.end();
		}

		QueueData const & front()const noexcept
		{
			return *m_allQueuesData.front();
		}

		QueueFamilyFlags familySupport;
		uint32_t familyIndex;

	private:
		struct QueueThreadData
		{
			QueueThreadData() = default;

			QueueData const * data{};
			uint32_t count{};
#ifndef NDEBUG
			castor::String callstack{};
#endif
		};

		castor::Vector< QueueDataPtr > m_allQueuesData;

		mutable castor::Mutex m_mutex;
		mutable castor::Vector< QueueData const * > m_remainingQueuesData;
		castor::Map< std::thread::id, QueueThreadData > m_busyQueues;
	};
	using QueueFamilies = castor::Vector< QueuesData >;

	struct RenderDevice
	{
		C3D_API RenderDevice( RenderSystem & renderSystem
			, ashes::PhysicalDevice const & gpu
			, AshPluginDescription const & desc
			, Extensions deviceExtensions );
		C3D_API ~RenderDevice()noexcept;

		C3D_API VkFormat selectSuitableDepthFormat( VkFormatFeatureFlags requiredFeatures )const;
		C3D_API VkFormat selectSuitableStencilFormat( VkFormatFeatureFlags requiredFeatures )const;
		C3D_API VkFormat selectSuitableDepthStencilFormat( VkFormatFeatureFlags requiredFeatures )const;
		C3D_API VkFormat selectSmallestFormatRSFloatFormat( VkFormatFeatureFlags requiredFeatures )const;
		C3D_API VkFormat selectSmallestFormatRGSFloatFormat( VkFormatFeatureFlags requiredFeatures )const;
		C3D_API VkFormat selectSmallestFormatRGBUFloatFormat( VkFormatFeatureFlags requiredFeatures )const;
		C3D_API VkFormat selectSmallestFormatRGBSFloatFormat( VkFormatFeatureFlags requiredFeatures )const;
		C3D_API VkFormat selectSuitableFormat( castor::Vector< VkFormat > const & formats
			, VkFormatFeatureFlags requiredFeatures )const;
		C3D_API QueueDataWrapper graphicsData()const noexcept;
		C3D_API size_t graphicsQueueSize()const noexcept;
		C3D_API QueueData const * reserveGraphicsData()const noexcept;
		C3D_API void unreserveGraphicsData( QueueData const * queueData )const noexcept;
		C3D_API void putGraphicsData( QueueData const * queueData )const noexcept;
		C3D_API crg::GraphContext & makeContext()const noexcept;
		C3D_API bool hasExtension( castor::MbStringView name )const noexcept;
		C3D_API bool hasTerminateInvocation()const noexcept;
		C3D_API bool hasDemoteToHelperInvocation()const noexcept;
		C3D_API bool hasMeshAndTaskShaders()const noexcept;
		C3D_API bool hasMeshShaders()const noexcept;
		C3D_API bool hasTaskShaders()const noexcept;
		C3D_API bool hasAtomicFloatAdd()const noexcept;
		C3D_API bool hasBufferDeviceAddress()const noexcept;
		C3D_API bool hasRayTracing()const noexcept;
		C3D_API bool hasBindless()const noexcept;
		C3D_API uint32_t getMaxBindlessSampled()const noexcept;
		C3D_API uint32_t getMaxBindlessStorage()const noexcept;
		C3D_API void fillGPUMeshInformations( GpuInformations & gpuInformations )const noexcept;

		bool prefersMeshShaderEXT()const noexcept
		{
			return m_prefersMeshShaderEXT;
		}

#if VK_NV_mesh_shader
		VkPhysicalDeviceMeshShaderPropertiesNV const & getMeshPropertiesNV()const noexcept
		{
			return m_meshShaderPropertiesNV;
		}
#endif
#if VK_EXT_mesh_shader
		VkPhysicalDeviceMeshShaderPropertiesEXT const & getMeshPropertiesEXT()const noexcept
		{
			return m_meshShaderPropertiesEXT;
		}
#endif
		bool hasComputeShaderDerivatives()const noexcept
		{
#if VK_NV_compute_shader_derivatives
			return m_computeDerivativesFeatures.computeDerivativeGroupQuads == VK_TRUE;
#else
			return false;
#endif
		}

		ashes::Device const * operator->()const noexcept
		{
			return device.get();
		}

		ashes::Device * operator->()noexcept
		{
			return device.get();
		}

		ashes::Device & operator*()noexcept
		{
			return *device;
		}

		ashes::Device const & operator*()const noexcept
		{
			return *device;
		}

		uint32_t getGraphicsQueueFamilyIndex()const noexcept
		{
			CU_Require( m_preferredGraphicsQueue );
			return m_preferredGraphicsQueue->familyIndex;
		}

		uint32_t getComputeQueueFamilyIndex()const noexcept
		{
			CU_Require( m_preferredComputeQueue );
			return m_preferredComputeQueue->familyIndex;
		}

		uint32_t getTransferQueueFamilyIndex()const noexcept
		{
			CU_Require( m_preferredTransferQueue );
			return m_preferredTransferQueue->familyIndex;
		}

		RenderSystem & renderSystem;
		ashes::PhysicalDevice const & gpu;
		AshPluginDescription const & desc;
		VkPhysicalDeviceMemoryProperties memoryProperties{};
		VkPhysicalDeviceFeatures features{};
		VkPhysicalDeviceProperties properties{};
		QueueFamilies queueFamilies;
		ashes::DevicePtr device;
		ashes::CommandPool * computeCommandPool{};
		ashes::CommandPool * transferCommandPool{};
		ashes::Queue * computeQueue{};
		ashes::Queue * transferQueue{};
		GpuBufferPoolUPtr bufferPool;
		VertexBufferPoolUPtr vertexPools;
		IndexBufferPoolUPtr indexPools;
		ObjectBufferPoolUPtr geometryPools;
		UniformBufferPoolUPtr uboPool;

	private:
		bool doTryAddExtension( castor::MbString const & name
			, void * pFeature = nullptr
			, void * pProperty = nullptr );

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#if VK_VERSION_1_3
		VkPhysicalDeviceVulkan13Features m_features13{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES
			, nullptr
			, {} };
		VkPhysicalDeviceVulkan13Properties m_properties13{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES
			, nullptr
			, {} };
#endif
#if VK_VERSION_1_2
		VkPhysicalDeviceVulkan11Features m_features11{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES
			, nullptr
			, {} };
		VkPhysicalDeviceVulkan11Properties m_properties11{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES
			, nullptr
			, {} };
		VkPhysicalDeviceVulkan12Features m_features12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES
			, nullptr
			, {} };
		VkPhysicalDeviceVulkan12Properties m_properties12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES
			, nullptr
			, {} };
#endif
		VkPhysicalDeviceShaderDrawParametersFeatures m_drawParamsFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES
			, nullptr
			, {} };
#if VK_VERSION_1_1
		VkPhysicalDeviceFeatures2 m_features2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2
			, nullptr
			, {} };
		VkPhysicalDeviceProperties2 m_properties2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2
			, nullptr
			, {} };
#endif
#if VK_KHR_8bit_storage
		VkPhysicalDevice8BitStorageFeaturesKHR m_8bitFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR };
#endif
#if VK_KHR_16bit_storage
		VkPhysicalDevice16BitStorageFeaturesKHR m_16bitFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR };
#endif
#if VK_KHR_shader_float16_int8
		VkPhysicalDeviceShaderFloat16Int8FeaturesKHR m_f16i8bitFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR };
#endif
#if VK_KHR_acceleration_structure
		VkPhysicalDeviceAccelerationStructureFeaturesKHR m_accelFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR
			, nullptr
			, {} };
		VkPhysicalDeviceAccelerationStructurePropertiesKHR m_accelProperties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR
			, nullptr
			, {} };
#endif
#if VK_KHR_ray_tracing_pipeline
		VkPhysicalDeviceRayTracingPipelineFeaturesKHR m_rtPipelineFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR
			, nullptr
			, {} };
		VkPhysicalDeviceRayTracingPipelinePropertiesKHR m_rtPipelineProperties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR
			, nullptr
			, {} };
#endif
#if VK_EXT_descriptor_indexing
		VkPhysicalDeviceDescriptorIndexingFeaturesEXT m_descriptorIndexingFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT
			, nullptr
			, {} };
		VkPhysicalDeviceDescriptorIndexingPropertiesEXT m_descriptorIndexingProperties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT
			, nullptr
			, {} };
#endif
#if VK_KHR_shader_terminate_invocation
		VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR m_terminateInvocationFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES_KHR
			, nullptr
			, {} };
#endif
#if VK_EXT_shader_demote_to_helper_invocation
		VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT m_demoteToHelperInvocationFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES_EXT
			, nullptr
			, {} };
#endif
#if VK_NV_mesh_shader
#if C3D_UseMeshShaders
		VkPhysicalDeviceMeshShaderFeaturesNV m_meshShaderFeaturesNV{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV
			, nullptr
			, {} };
#endif
		VkPhysicalDeviceMeshShaderPropertiesNV m_meshShaderPropertiesNV{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_NV
			, nullptr
			, {} };
#endif
#if VK_EXT_mesh_shader
#if C3D_UseMeshShaders
		VkPhysicalDeviceMeshShaderFeaturesEXT m_meshShaderFeaturesEXT{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT
			, nullptr
			, {} };
#endif
		VkPhysicalDeviceMeshShaderPropertiesEXT m_meshShaderPropertiesEXT{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_EXT
			, nullptr
			, {} };
#endif
#if VK_NV_compute_shader_derivatives
		VkPhysicalDeviceComputeShaderDerivativesFeaturesNV m_computeDerivativesFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COMPUTE_SHADER_DERIVATIVES_FEATURES_NV
			, nullptr
			, {} };
#endif
#if VK_EXT_shader_atomic_float
		VkPhysicalDeviceShaderAtomicFloatFeaturesEXT m_atomicFloatAddFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT
			, nullptr
			, {} };
#endif
#if VK_KHR_buffer_device_address
		VkPhysicalDeviceBufferDeviceAddressFeaturesKHR m_bufferDeviceAddressFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES
			, nullptr
			, {} };
#endif
#pragma GCC diagnostic pop

		bool m_prefersMeshShaderEXT{ true };
		Extensions m_deviceExtensions;
		ashes::VkExtensionPropertiesArray m_availableExtensions;
		QueuesData * m_preferredGraphicsQueue{};
		QueuesData * m_preferredComputeQueue{};
		QueuesData * m_preferredTransferQueue{};
		bool m_hasFeatures12{};
		bool m_hasFeatures13{};

		using GraphContextPtr = castor::RawUniquePtr< crg::GraphContext >;
		using ThreadGraphContexts = castor::Map< std::thread::id, GraphContextPtr >;
		mutable castor::Mutex m_mutex;
		mutable ThreadGraphContexts m_contexts;
	};
}

#endif
