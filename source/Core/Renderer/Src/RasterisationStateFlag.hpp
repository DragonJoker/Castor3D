/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_RasterisationStateFlag_HPP___
#define ___Renderer_RasterisationStateFlag_HPP___
#pragma once

#include <Design/FlagCombination.hpp>

namespace renderer
{
	/**
	*\brief
	*	Enumération des types d'accès.
	*/
	enum class RasterisationStateFlag
		: uint32_t
	{
	};
	IMPLEMENT_FLAGS( RasterisationStateFlag )
}

#endif
