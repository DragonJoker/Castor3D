/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_QueueFamilyProperties_HPP___
#define ___Renderer_QueueFamilyProperties_HPP___
#pragma once

#include "Extent3D.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Provides information about a queue family.
	*\~french
	*\brief
	*	Fournit des information à propos d'une famille de file.
	*/
	struct QueueFamilyProperties
	{
		QueueFlags queueFlags;
		uint32_t queueCount;
		uint32_t timestampValidBits;
		Extent3D minImageTransferGranularity;
	};
}

#endif
