/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_BorderColour_HPP___
#define ___Renderer_BorderColour_HPP___
#pragma once

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
		Utils_EnumBounds( eFloatTransparentBlack )
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
	inline std::string getName( BorderColour value )
	{
		switch ( value )
		{
		case BorderColour::eFloatTransparentBlack:
			return "float_transparent_black";

		case BorderColour::eIntTransparentBlack:
			return "int_transparent_black";

		case BorderColour::eFloatOpaqueBlack:
			return "float_opaque_black";

		case BorderColour::eIntOpaqueBlack:
			return "int_opaque_black";

		case BorderColour::eFloatOpaqueWhite:
			return "float_opaque_white";

		case BorderColour::eIntOpaqueWhite:
			return "int_opaque_white";

		default:
			assert( false && "Unsupported BorderColour." );
			throw std::runtime_error{ "Unsupported BorderColour" };
		}

		return 0;
	}
}

#endif
