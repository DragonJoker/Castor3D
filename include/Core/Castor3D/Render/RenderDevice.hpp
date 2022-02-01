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

	struct Feature
	{
		std::string extName;
		VkStructure * featureStruct;
	};

	using FeatureArray = std::vector< Feature >;

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
		C3D_API void addExtension( std::string const & extName );
		/**
		*\~english
		*\brief
		*	Adds an extension, and it's optional feature structure, that will be queried on physical device.
		*\~french
		*\brief
		*	Ajoute une extension, et sa feature structure optionnelle, qui sera remplie via le physical device.
		*/
		C3D_API void addExtension( std::string const & extName
			, VkStructure * featureStruct );
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
			if ( featureStruct )
			{
				m_extensions.push_back( { ""
					, reinterpret_cast< VkStructure * >( featureStruct ) } );
			}
		}

		ashes::StringArray const & getExtensionsNames()const
		{
			return m_extensionsNames;
		}

		bool isEmpty()const
		{
			return m_extensions.empty();
		}

		size_t getSize()const
		{
			return m_extensions.size();
		}

		Feature const & operator[]( size_t index )const
		{
			return m_extensions[index];
		}

	private:
		ashes::StringArray m_extensionsNames;
		FeatureArray m_extensions;
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
		C3D_API QueueData( QueueData && ) = default;
		C3D_API QueueData & operator=( QueueData && ) = default;

		explicit QueueData( QueuesData const * parent )
			: parent{ parent }
		{
		}

		QueuesData const * parent;
		ashes::QueuePtr queue;
		ashes::CommandPoolPtr commandPool;
	};
	using QueueDataPtr = std::unique_ptr< QueueData >;

	struct QueueDataWrapper
	{
		QueueDataWrapper( QueueDataWrapper const & ) = delete;
		QueueDataWrapper & operator=( QueueDataWrapper const & ) = delete;

		C3D_API QueueDataWrapper( QueueDataWrapper && rhs );
		C3D_API QueueDataWrapper & operator=( QueueDataWrapper && rhs );
		C3D_API QueueDataWrapper( QueuesData * parent );
		C3D_API ~QueueDataWrapper();

		QueueData const * operator->()
		{
			return data;
		}

		QueueData const & operator*()
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
		C3D_API QueuesData( QueuesData && rhs );
		C3D_API QueuesData & operator=( QueuesData && rhs );

		C3D_API QueuesData( QueueFamilyFlags familySupport = QueueFamilyFlag::eNone
			, uint32_t familyIndex = 0xFFFFFFFFu );

		C3D_API void initialise( ashes::Device const & device );
		C3D_API void setQueueSize( size_t count );
		C3D_API QueueData const * reserveQueue()const;
		C3D_API QueueData const * getQueue();
		C3D_API void putQueue( QueueData const * queue );

		size_t getQueueSize()const
		{
			return m_allQueuesData.size();
		}

		auto begin()const
		{
			return m_allQueuesData.begin();
		}

		auto end()const
		{
			return m_allQueuesData.end();
		}

		QueueData const & front()const
		{
			return *m_allQueuesData.front();
		}

		QueueFamilyFlags familySupport;
		uint32_t familyIndex;

	private:
		struct QueueThreadData
		{
			QueueData const * data;
			uint32_t count;
		};

		std::vector< QueueDataPtr > m_allQueuesData;

		mutable std::mutex m_mutex;
		mutable std::vector< QueueData const * > m_remainingQueuesData;
		std::map< std::thread::id, QueueThreadData > m_busyQueues;
	};
	using QueueFamilies = std::vector< QueuesData >;

	struct RenderDevice
	{
		C3D_API RenderDevice( RenderSystem & renderSystem
			, ashes::PhysicalDevice const & gpu
			, AshPluginDescription const & desc
			, Extensions deviceExtensions );
		C3D_API ~RenderDevice();

		C3D_API VkFormat selectSuitableDepthFormat( VkFormatFeatureFlags requiredFeatures )const;
		C3D_API VkFormat selectSuitableStencilFormat( VkFormatFeatureFlags requiredFeatures )const;
		C3D_API VkFormat selectSuitableDepthStencilFormat( VkFormatFeatureFlags requiredFeatures )const;
		C3D_API VkFormat selectSuitableFormat( std::vector< VkFormat > const & formats
			, VkFormatFeatureFlags requiredFeatures )const;
		C3D_API QueueDataWrapper graphicsData()const;
		C3D_API QueueData const * reserveGraphicsData()const;
		C3D_API crg::GraphContext & makeContext()const;
		C3D_API bool hasExtension( std::string_view const & name )const;
		C3D_API bool hasTerminateInvocation()const;
		C3D_API bool hasDemoteToHelperInvocation()const;
		C3D_API bool hasMeshAndTaskShaders()const;
		C3D_API bool hasAtomicFloatAdd()const;
		C3D_API bool hasBufferDeviceAddress()const;
		C3D_API bool hasRayTracing()const;

		ashes::Device const * operator->()const
		{
			return device.get();
		}

		ashes::Device * operator->()
		{
			return device.get();
		}

		ashes::Device & operator*()
		{
			return *device;
		}

		ashes::Device const & operator*()const
		{
			return *device;
		}

		uint32_t getGraphicsQueueFamilyIndex()const
		{
			CU_Require( m_preferredGraphicsQueue );
			return m_preferredGraphicsQueue->familyIndex;
		}

		uint32_t getComputeQueueFamilyIndex()const
		{
			CU_Require( m_preferredComputeQueue );
			return m_preferredComputeQueue->familyIndex;
		}

		uint32_t getTransferQueueFamilyIndex()const
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
		GpuBufferPoolSPtr bufferPool;
		UniformBufferPoolsSPtr uboPools;

	private:
#if VK_VERSION_1_2
		VkPhysicalDeviceVulkan12Features m_features12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES
			, nullptr
			, {} };
		VkPhysicalDeviceVulkan11Features m_features11{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES
			, nullptr
			, {} };
#endif
		VkPhysicalDeviceShaderDrawParametersFeatures drawParamsFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES
			, nullptr
			, {} };
#if VK_VERSION_1_1
		VkPhysicalDeviceFeatures2 m_features2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2
			, nullptr
			, {} };
#endif
#if VK_KHR_acceleration_structure
		VkPhysicalDeviceAccelerationStructureFeaturesKHR m_accelFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR
			, nullptr
			, {} };
#endif
#if VK_KHR_ray_tracing_pipeline
		VkPhysicalDeviceRayTracingPipelineFeaturesKHR m_rtPipelineFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR
			, nullptr
			, {} };
#endif
#if VK_EXT_descriptor_indexing
		VkPhysicalDeviceDescriptorIndexingFeaturesEXT m_descriptorIndexingFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES
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
		VkPhysicalDeviceMeshShaderFeaturesNV m_meshShaderFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV
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
		Extensions m_deviceExtensions;
		QueuesData * m_preferredGraphicsQueue{};
		QueuesData * m_preferredComputeQueue{};
		QueuesData * m_preferredTransferQueue{};

		using GraphContextPtr = std::unique_ptr< crg::GraphContext >;
		using ThreadGraphContexts = std::map< std::thread::id, GraphContextPtr >;
		mutable std::mutex m_mutex;
		mutable ThreadGraphContexts m_contexts;
	};
}

#endif
