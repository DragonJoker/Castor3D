/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Extent2D_HPP___
#define ___Renderer_Extent2D_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies a two dimensional extent.
	*\~french
	*\brief
	*	Définit une étendue en 2 dimensions.
	*/
	struct Extent2D
	{
		uint32_t width;
		uint32_t height;
	};

	inline bool operator==( Extent2D const & lhs, Extent2D const & rhs )
	{
		return lhs.width == rhs.width
			&& lhs.height == rhs.height;
	}

	inline bool operator!=( Extent2D const & lhs, Extent2D const & rhs )
	{
		return !( lhs == rhs );
	}
}

#endif
