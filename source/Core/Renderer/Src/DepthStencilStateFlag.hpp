/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_DepthStencilStateFlag_HPP___
#define ___Renderer_DepthStencilStateFlag_HPP___
#pragma once

#include <Design/FlagCombination.hpp>

namespace renderer
{
	/**
	*\brief
	*	Enumération des types d'accès.
	*/
	enum class DepthStencilStateFlag
		: uint32_t
	{
	};
	IMPLEMENT_FLAGS( DepthStencilStateFlag )
}

#endif
