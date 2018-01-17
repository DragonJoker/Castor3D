/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_QueryControlFlag_HPP___
#define ___Renderer_QueryControlFlag_HPP___
#pragma once

#include <Design/FlagCombination.hpp>

namespace renderer
{
	/**
	*\brief
	*	Enumération des contrôles de requête d'occlusion.
	*/
	enum class QueryControlFlag
		: uint32_t
	{
		ePrecise = 0x00000001,
	};
	IMPLEMENT_FLAGS( QueryControlFlag )
}

#endif
