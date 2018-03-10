/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Offset2D_HPP___
#define ___Renderer_Offset2D_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies a two dimensional offset.
	*\~french
	*\brief
	*	Définit un décalage en 2 dimensions.
	*/
	struct Offset2D
	{
		int32_t x;
		int32_t y;
	};

	inline bool operator==( Offset2D const & lhs, Offset2D const & rhs )
	{
		return lhs.x == rhs.x
			&& lhs.y == rhs.y;
	}

	inline bool operator!=( Offset2D const & lhs, Offset2D const & rhs )
	{
		return !( lhs == rhs );
	}
}

#endif
