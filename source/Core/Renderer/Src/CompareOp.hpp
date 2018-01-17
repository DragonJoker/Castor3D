/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_CompareOp_HPP___
#define ___Renderer_CompareOp_HPP___
#pragma once

#include <Design/FlagCombination.hpp>

namespace renderer
{
	/**
	*\brief
	*	Opérateurs de comparaison.
	*/
	enum class CompareOp
		: uint32_t
	{
		eNever,
		eLess,
		eEqual,
		eLessEqual,
		eGreater,
		eNotEqual,
		eGreaterEqual,
		eAlways,
	};
}

#endif
