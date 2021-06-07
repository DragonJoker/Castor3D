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

#include <array>
#include <vector>

namespace castor3d
{
	enum class QueueFamilyFlag
	{
		eNone = 0x00,
		ePresent = 0x01,
		eGraphics = 0x02,
		eTransfer = 0x04,
		eCompute = 0x08,
	};
	CU_ImplementFlags( QueueFamilyFlag )

	struct RenderDevice
	{
		struct QueueData
		{
			QueueFamilyFlags familySupport{};
			uint32_t familyIndex{ 0xFFFFFFFFu };
			ashes::CommandPoolPtr commandPool{ nullptr };
			std::vector< ashes::QueuePtr > queues;
		};
		using QueueFamilies = std::vector< QueueData >;

		C3D_API RenderDevice( RenderSystem & renderSystem
			, ashes::PhysicalDevice const & gpu
			, AshPluginDescription const & desc );
		C3D_API ~RenderDevice();

		C3D_API VkFormat selectSuitableDepthFormat( VkFormatFeatureFlags requiredFeatures )const;
		C3D_API VkFormat selectSuitableStencilFormat( VkFormatFeatureFlags requiredFeatures )const;
		C3D_API VkFormat selectSuitableDepthStencilFormat( VkFormatFeatureFlags requiredFeatures )const;
		C3D_API VkFormat selectSuitableFormat( std::vector< VkFormat > const & formats
			, VkFormatFeatureFlags requiredFeatures )const;
		C3D_API crg::GraphContext makeContext()const;

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
		QueueData * m_preferredGraphicsQueue{};
		QueueData * m_preferredComputeQueue{};
		QueueData * m_preferredTransferQueue{};
		ashes::CommandPool * graphicsCommandPool{};
		ashes::CommandPool * computeCommandPool{};
		ashes::CommandPool * transferCommandPool{};
		ashes::Queue * graphicsQueue{};
		ashes::Queue * computeQueue{};
		ashes::Queue * transferQueue{};
		GpuBufferPoolSPtr bufferPool;
		UniformBufferPoolsSPtr uboPools;
	};
}

#endif
