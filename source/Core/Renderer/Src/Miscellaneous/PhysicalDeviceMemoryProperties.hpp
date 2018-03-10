/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_PhysicalDeviceMemoryProperties_HPP___
#define ___Renderer_PhysicalDeviceMemoryProperties_HPP___
#pragma once

#include "MemoryHeap.hpp"
#include "MemoryType.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies a physical device memory properties.
	*\~french
	*\brief
	*	Définit les propriétés mémoire d'un périphérique physique.
	*/
	struct PhysicalDeviceMemoryProperties
	{
		std::vector< MemoryType > memoryTypes;
		std::vector< MemoryHeap > memoryHeaps;
	};
}

#endif
