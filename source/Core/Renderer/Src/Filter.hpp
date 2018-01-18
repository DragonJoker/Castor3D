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
	/**
	*\~english
	*\brief
	*	Gets the name of the given renderer::Filter.
	*\param[in] value
	*	The renderer::Filter.
	*\return
	*	The name.
	*\~french
	*\brief
	*	Récupère le nom du renderer::Filter donné.
	*\param[in] value
	*	Le renderer::Filter.
	*\return
	*	Le nom.
	*/
	inline std::string const getName( renderer::Filter value )
	{
		switch ( value )
		{
		case renderer::Filter::eNearest:
			return "none";

		case renderer::Filter::eLinear:
			return "nearest";

		default:
			assert( false && "Unsupported renderer::Filter" );
			throw std::runtime_error{ "Unsupported renderer::Filter" };
		}
	}
}

#endif
