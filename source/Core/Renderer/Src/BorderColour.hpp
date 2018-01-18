/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_BorderColour_HPP___
#define ___Renderer_BorderColour_HPP___
#pragma once

#include <CastorUtilsPrerequisites.hpp>

namespace renderer
{
	/**
	*\brief
	*	Enumération des types de texture.
	*/
	enum class BorderColour
		: uint32_t
	{
		eFloatTransparentBlack = 0,
		eIntTransparentBlack = 1,
		eFloatOpaqueBlack = 2,
		eIntOpaqueBlack = 3,
		eFloatOpaqueWhite = 4,
		eIntOpaqueWhite = 5,
		CASTOR_SCOPED_ENUM_BOUNDS( eFloatTransparentBlack )
	};
	/**
	*\~english
	*\brief
	*	Gets the name of the given renderer::BorderColour.
	*\param[in] value
	*	The renderer::BorderColour.
	*\return
	*	The name.
	*\~french
	*\brief
	*	Récupère le nom du renderer::BorderColour donné.
	*\param[in] value
	*	Le renderer::BorderColour.
	*\return
	*	Le nom.
	*/
	inline std::string const getName( renderer::BorderColour value )
	{
		switch ( value )
		{
		case renderer::BorderColour::eFloatTransparentBlack:
			return "float_transparent_black";

		case renderer::BorderColour::eIntTransparentBlack:
			return "int_transparent_black";

		case renderer::BorderColour::eFloatOpaqueBlack:
			return "float_opaque_black";

		case renderer::BorderColour::eIntOpaqueBlack:
			return "int_opaque_black";

		case renderer::BorderColour::eFloatOpaqueWhite:
			return "float_opaque_white";

		case renderer::BorderColour::eIntOpaqueWhite:
			return "int_opaque_white";

		default:
			assert( false && "Unsupported renderer::BorderColour" );
			throw std::runtime_error{ "Unsupported renderer::BorderColour" };
		}
	}
}

#endif
