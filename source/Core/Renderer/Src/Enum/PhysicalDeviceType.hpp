/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_PhysicalDeviceType_HPP___
#define ___Renderer_PhysicalDeviceType_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Enumération des types de périphérique physique.
	*/
	enum class PhysicalDeviceType
		: uint32_t
	{
		eOther = 0,
		eIntegratedGpu = 1,
		eDiscreteGpu = 2,
		eVirtualGpu = 3,
		eCpu = 4,
	};
}

#endif
