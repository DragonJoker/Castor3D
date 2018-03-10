/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_PhysicalDeviceProperties_HPP___
#define ___Renderer_PhysicalDeviceProperties_HPP___
#pragma once

#include "PhysicalDeviceLimits.hpp"
#include "PhysicalDeviceSparseProperties.hpp"

namespace renderer
{
	static uint32_t constexpr UuidSize = 16u;
	/**
	*\~english
	*\brief
	*	Specifies a physical device properties.
	*\~french
	*\brief
	*	Définit les propriétés d'un périphérique physique.
	*/
	struct PhysicalDeviceProperties
	{
		uint32_t apiVersion;
		uint32_t driverVersion;
		uint32_t vendorID;
		uint32_t deviceID;
		PhysicalDeviceType deviceType;
		std::string deviceName;
		uint8_t pipelineCacheUUID[UuidSize];
		PhysicalDeviceLimits limits;
		PhysicalDeviceSparseProperties sparseProperties;
	};
}

#endif
