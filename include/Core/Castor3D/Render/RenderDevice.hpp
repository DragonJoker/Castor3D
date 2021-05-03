/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderDevice_H___
#define ___C3D_RenderDevice_H___

#include "RenderModule.hpp"
#include "Castor3D/Buffer/BufferModule.hpp"

#include <ashespp/Command/CommandPool.hpp>
#include <ashespp/Sync/Queue.hpp>

#include <array>
#include <vector>

namespace castor3d
{
	struct RenderDevice
	{
		C3D_API RenderDevice( RenderSystem & renderSystem
			, AshPluginDescription const & desc
			, ashes::Surface const & surface );

		C3D_API VkFormat selectSuitableDepthFormat( VkFormatFeatureFlags requiredFeatures )const;
		C3D_API VkFormat selectSuitableStencilFormat( VkFormatFeatureFlags requiredFeatures )const;
		C3D_API VkFormat selectSuitableDepthStencilFormat( VkFormatFeatureFlags requiredFeatures )const;
		C3D_API VkFormat selectSuitableFormat( std::vector< VkFormat > const & formats
			, VkFormatFeatureFlags requiredFeatures )const;

		inline ashes::Device const * operator->()const
		{
			return device.get();
		}

		inline ashes::Device * operator->()
		{
			return device.get();
		}

		inline ashes::Device & operator*()
		{
			return *device;
		}

		inline ashes::Device const & operator*()const
		{
			return *device;
		}

		inline uint32_t getGraphicsQueueFamilyIndex()const
		{
			return queueFamiliesIndex[GraphicsIdx];
		}

		inline uint32_t getPresentQueueFamilyIndex()const
		{
			return queueFamiliesIndex[PresentIdx];
		}

		inline uint32_t getComputeQueueFamilyIndex()const
		{
			return queueFamiliesIndex[ComputeIdx];
		}

		inline uint32_t getTransferQueueFamilyIndex()const
		{
			return queueFamiliesIndex[TransferIdx];
		}

		RenderSystem & renderSystem;
		ashes::Surface const & surface;
		ashes::PhysicalDevice const & gpu;
		AshPluginDescription const & desc;
		VkPhysicalDeviceMemoryProperties memoryProperties{};
		VkPhysicalDeviceFeatures features{};
		VkPhysicalDeviceProperties properties{};
		std::array< uint32_t, 4u > queueFamiliesIndex;
		ashes::DevicePtr device;
		std::vector< ashes::CommandPoolPtr > commandPools;
		ashes::CommandPool * presentCommandPool{ nullptr };
		ashes::CommandPool * graphicsCommandPool{ nullptr };
		ashes::CommandPool * computeCommandPool{ nullptr };
		ashes::CommandPool * transferCommandPool{ nullptr };
		ashes::QueuePtr presentQueue{ nullptr };
		ashes::QueuePtr graphicsQueue{ nullptr };
		ashes::QueuePtr computeQueue{ nullptr };
		ashes::QueuePtr transferQueue{ nullptr };
		GpuBufferPoolSPtr bufferPool;
		UniformBufferPoolsSPtr uboPools;

		static constexpr size_t GraphicsIdx = 0u;
		static constexpr size_t PresentIdx = 1u;
		static constexpr size_t ComputeIdx = 2u;
		static constexpr size_t TransferIdx = 3u;
	};
}

#endif
