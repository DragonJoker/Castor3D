/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderDevice_H___
#define ___C3D_RenderDevice_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

#include <ashespp/Command/CommandPool.hpp>
#include <ashespp/Command/Queue.hpp>

#include <vector>

namespace castor3d
{
	struct RenderDevice
	{
		RenderDevice( RenderSystem & renderSystem
			, ashes::PhysicalDevice const & gpu
			, AshPluginDescription const & desc
			, ashes::SurfacePtr surface );

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
		ashes::DevicePtr device;
		std::vector< ashes::CommandPoolPtr > commandPools;
		uint32_t presentQueueFamilyIndex;
		uint32_t graphicsQueueFamilyIndex;
		uint32_t computeQueueFamilyIndex;
		uint32_t transferQueueFamilyIndex;
		ashes::CommandPool * graphicsCommandPool{ nullptr };
		ashes::CommandPool * computeCommandPool{ nullptr };
		ashes::CommandPool * presentCommandPool{ nullptr };
		ashes::CommandPool * transferCommandPool{ nullptr };
		ashes::QueuePtr graphicsQueue{ nullptr };
		ashes::QueuePtr computeQueue{ nullptr };
		ashes::QueuePtr presentQueue{ nullptr };
		ashes::QueuePtr transferQueue{ nullptr };
	};
}

#endif
