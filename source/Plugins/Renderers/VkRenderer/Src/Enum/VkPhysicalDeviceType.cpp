#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkPhysicalDeviceType convert( renderer::PhysicalDeviceType const & value )
	{
		switch ( value )
		{
		case renderer::PhysicalDeviceType::eOther:
			return VK_PHYSICAL_DEVICE_TYPE_OTHER;

		case renderer::PhysicalDeviceType::eIntegratedGpu:
			return VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;

		case renderer::PhysicalDeviceType::eDiscreteGpu:
			return VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;

		case renderer::PhysicalDeviceType::eVirtualGpu:
			return VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU;

		case renderer::PhysicalDeviceType::eCpu:
			return VK_PHYSICAL_DEVICE_TYPE_CPU;

		default:
			assert( false );
			return VK_PHYSICAL_DEVICE_TYPE_OTHER;
		}
	}

	renderer::PhysicalDeviceType convertPhysicalDeviceType( VkPhysicalDeviceType const & value )
	{
		switch ( value )
		{
		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
			return renderer::PhysicalDeviceType::eOther;

		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			return renderer::PhysicalDeviceType::eIntegratedGpu;

		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			return renderer::PhysicalDeviceType::eDiscreteGpu;

		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			return renderer::PhysicalDeviceType::eVirtualGpu;

		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			return renderer::PhysicalDeviceType::eCpu;

		default:
			assert( false );
			return renderer::PhysicalDeviceType::eOther;
		}
	}
}
