/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_LogicOp_HPP___
#define ___Renderer_LogicOp_HPP___
#pragma once

#include <Design/FlagCombination.hpp>

namespace renderer
{
	/**
	*\brief
	*	Opérateurs logiques.
	*/
	enum class LogicOp
		: uint32_t
	{
		eClear,
		eAnd,
		eAndReverse,
		eCopy,
		eAndInverted,
		eNoOp,
		eXor,
		eOr,
		eNor,
		eEquivalent,
		eInvert,
		eOrReverse,
		eCopyInverted,
		eOrInverted,
		eNand,
		eSet,
	};
}

#endif
