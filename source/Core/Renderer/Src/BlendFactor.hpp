/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_BlendFactor_HPP___
#define ___Renderer_BlendFactor_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Les facteurs de mélange.
	*/
	enum class BlendFactor
		: uint32_t
	{
		eZero,
		eOne,
		eSrcColour,
		eInvSrcColour,
		eDstColour,
		eInvDstColour,
		eSrcAlpha,
		eInvSrcAlpha,
		eDstAlpha,
		eInvDstAlpha,
		eConstantColour,
		eInvConstantColour,
		eConstantAlpha,
		eInvConstantAlpha,
		eSrcAlphaSaturate,
		eSrc1Colour,
		eInvSrc1Colour,
		eSrc1Alpha,
		eInvSrc1Alpha,
	};
}

#endif
