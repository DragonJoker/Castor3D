/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_MemoryType_HPP___
#define ___Renderer_MemoryType_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies a memory type.
	*\~french
	*\brief
	*	Définit un type de mémoire.
	*/
	struct MemoryType
	{
		MemoryPropertyFlags propertyFlags;
		uint32_t heapIndex;
	};
}

#endif
