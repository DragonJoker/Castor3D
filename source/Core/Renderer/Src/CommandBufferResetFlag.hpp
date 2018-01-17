/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_CommandBufferResetFlag_HPP___
#define ___Renderer_CommandBufferResetFlag_HPP___
#pragma once

#include <Design/FlagCombination.hpp>

namespace renderer
{
	/**
	*\brief
	*	Enumération des indicateurs de réinitalisation de tampon de commandes.
	*/
	enum class CommandBufferResetFlag
		: uint32_t
	{
		eReleaseResources = 0x00000001,
	};
	IMPLEMENT_FLAGS( CommandBufferResetFlag )
}

#endif
