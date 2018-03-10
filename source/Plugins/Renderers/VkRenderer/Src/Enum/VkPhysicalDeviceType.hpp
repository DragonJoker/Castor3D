/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#pragma once

#include <RendererPrerequisites.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::PhysicalDeviceType en VkPhysicalDeviceType.
	*\param[in] value
	*	Le renderer::PhysicalDeviceType.
	*\return
	*	Le VkPhysicalDeviceType.
	*/
	VkPhysicalDeviceType convert( renderer::PhysicalDeviceType const & value );
	/**
	*\brief
	*	Convertit un VkPhysicalDeviceType en renderer::PhysicalDeviceType.
	*\param[in] value
	*	Le VkPhysicalDeviceType.
	*\return
	*	Le renderer::PhysicalDeviceType.
	*/
	renderer::PhysicalDeviceType convertPhysicalDeviceType( VkPhysicalDeviceType const & value );
}
