/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Offset3D_HPP___
#define ___Renderer_Offset3D_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies a three dimensional offset.
	*\~french
	*\brief
	*	Définit un décalage en 3 dimensions.
	*/
	struct Offset3D
	{
		int32_t x;
		int32_t y;
		int32_t z;
	};

	inline bool operator==( Offset3D const & lhs, Offset3D const & rhs )
	{
		return lhs.x == rhs.x
			&& lhs.y == rhs.y
			&& lhs.z == rhs.z;
	}

	inline bool operator!=( Offset3D const & lhs, Offset3D const & rhs )
	{
		return !( lhs == rhs );
	}
}

#endif
