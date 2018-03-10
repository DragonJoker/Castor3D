/*
This file belongs to RendererLib.
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
		Utils_EnumBounds( eNearest )
	};
	/**
	*\~english
	*\brief
	*	Gets the name of the given element type.
	*\param[in] value
	*	The element type.
	*\return
	*	The name.
	*\~french
	*\brief
	*	Récupère le nom du type d'élément donné.
	*\param[in] value
	*	Le type d'élément.
	*\return
	*	Le nom.
	*/
	inline std::string getName( Filter value )
	{
		switch ( value )
		{
		case Filter::eNearest:
			return "nearest";

		case Filter::eLinear:
			return "linear";

		default:
			assert( false && "Unsupported Filter." );
			throw std::runtime_error{ "Unsupported Filter" };
		}

		return 0;
	}
}

#endif
