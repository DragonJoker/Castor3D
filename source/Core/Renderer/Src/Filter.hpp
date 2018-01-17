/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Filter_HPP___
#define ___Renderer_Filter_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~french
	*\brief
	*	Enumération des filtres.
	*\~english
	*\brief
	*	Filters enumeration.
	*/
	enum class Filter
	{
		eNearest,
		eLinear,
		CASTOR_SCOPED_ENUM_BOUNDS( eNearest )
	};
}

#endif
