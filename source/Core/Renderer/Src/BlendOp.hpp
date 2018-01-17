/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_BlendOp_HPP___
#define ___Renderer_BlendOp_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Opérateurs de mélange.
	*/
	enum class BlendOp
		: uint32_t
	{
		eAdd,
		eSubtract,
		eReverseSubtract,
		eMin,
		eMax,
	};
}

#endif
