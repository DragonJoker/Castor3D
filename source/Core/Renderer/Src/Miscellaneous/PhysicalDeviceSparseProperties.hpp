/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_PhysicalDeviceSparseProperties_HPP___
#define ___Renderer_PhysicalDeviceSparseProperties_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies physical device sparse memory properties.
	*\~french
	*\brief
	*	Spécifie les propriétés de mémoire dispersée d'un périphérique physique.
	*/
	struct PhysicalDeviceSparseProperties
	{
		bool residencyStandard2DBlockShape;
		bool residencyStandard2DMultisampleBlockShape;
		bool residencyStandard3DBlockShape;
		bool residencyAlignedMipSize;
		bool residencyNonResidentStrict;
	};
}

#endif
