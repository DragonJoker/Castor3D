/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_TessellationState_HPP___
#define ___Renderer_TessellationState_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~french
	*\brief
	*	Un état de tessellation.
	*/
	struct TessellationState
	{
		TessellationStateFlags flags = 0u;
		uint32_t patchControlPoints = 0u;
	};

	inline bool operator==( TessellationState const & lhs, TessellationState const & rhs )
	{
		return lhs.flags == rhs.flags
			&& lhs.patchControlPoints == rhs.patchControlPoints;
	}

	inline bool operator!=( TessellationState const & lhs, TessellationState const & rhs )
	{
		return !( lhs == rhs );
	}
}

#endif
