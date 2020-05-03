/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderDevice_H___
#define ___C3D_RenderDevice_H___

#include "RenderModule.hpp"

#include <ashespp/Command/CommandPool.hpp>
#include <ashespp/Sync/Queue.hpp>

#include <vector>

namespace castor3d
{
	struct RenderDevice
	{
		C3D_API RenderDevice( RenderSystem & renderSystem
			, ashes::PhysicalDevice const & gpu
			, AshPluginDescription const & desc
			, ashes::SurfacePtr surface );

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

		RenderSystem & renderSystem;
		ashes::PhysicalDevice const & gpu;
		AshPluginDescription const & desc;
		ashes::SurfacePtr surface;
		VkPhysicalDeviceMemoryProperties memoryProperties{};
		VkPhysicalDeviceFeatures features{};
		VkPhysicalDeviceProperties properties{};
		uint32_t presentQueueFamilyIndex;
		uint32_t graphicsQueueFamilyIndex;
		uint32_t computeQueueFamilyIndex;
		uint32_t transferQueueFamilyIndex;
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
	};
}

#endif
