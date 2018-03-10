/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_MemoryRequirements_HPP___
#define ___Renderer_MemoryRequirements_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	enum class ResourceType
	{
		eImage,
		eBuffer,
	};
	/**
	*\~english
	*\brief
	*	Specifies memory requirements.
	*\~french
	*\brief
	*	Définit les exigences mémoire.
	*/
	struct MemoryRequirements
	{
		/**
		*\~english
		*\brief
		*	The resource type.
		*\~french
		*\brief
		*	Le type de ressource.
		*/
		ResourceType type;
		/**
		*\~english
		*\brief
		*	The size, in bytes, of the memory allocation required for the resource.
		*\~french
		*\brief
		*	La taille, en octets, de l'allocation mémoire requise pour la ressource.
		*/
		uint64_t size;
		/**
		*\~english
		*\brief
		*	The alignment, in bytes, of the offset within the allocation required for the resource.
		*\~french
		*\brief
		*	L'alignement, en octets, du décalage au sein de l'allocation requise pour la ressource.
		*/
		uint64_t alignment;
		/**
		*\~english
		*\brief
		*	A bitmask containing one bit set for every supported memory type for the resource.
		*	Bit \p i is set iff the memory type \p i in the renderer::PhysicalDeviceMemoryProperties structure for the physical device is supported for the resource.
		*\~french
		*\brief
		*	Un masque de bits contenant un bit défini pour chaque type de mémoire supporté pour la ressource.
		*	Le \p i-ème bit est défini ssi le \p i-ème type de mémoire dans la structure renderer::PhysicalDeviceMemoryProperties pour le périphérique physique est supporté pour la ressource.
		*/
		uint32_t memoryTypeBits;
	};
}

#endif
