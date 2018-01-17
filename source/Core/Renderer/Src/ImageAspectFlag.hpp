/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ImageAspectFlag_HPP___
#define ___Renderer_ImageAspectFlag_HPP___
#pragma once

#include <Design/FlagCombination.hpp>

namespace renderer
{
	/**
	*\brief
	*	Masques de bits décrivant les aspects d'une image.
	*/
	enum class ImageAspectFlag
		: uint32_t
	{
		eColour = 0x00000001,
		eDepth = 0x00000002,
		eStencil = 0x00000004,
		eMetaData = 0x00000008,
	};
	IMPLEMENT_FLAGS( ImageAspectFlag )
}

#endif
