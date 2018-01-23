/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_WrapMode_HPP___
#define ___Renderer_WrapMode_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Enumération des modes de wrap.
	*/
	enum class WrapMode
	{
		eRepeat,
		eMirroredRepeat,
		eClampToEdge,
		eClampToBorder,
		eMirrorClampToEdge,
		Utils_EnumBounds( eRepeat )
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
	inline std::string getName( WrapMode value )
	{
		switch ( value )
		{
		case WrapMode::eRepeat:
			return "repeat";

		case WrapMode::eMirroredRepeat:
			return "mirrored_repeat";

		case WrapMode::eClampToEdge:
			return "clamp_to_edge";

		case WrapMode::eClampToBorder:
			return "clamp_to_border";

		case WrapMode::eMirrorClampToEdge:
			return "mirrored_clamp_to_edge";

		default:
			assert( false && "Unsupported WrapMode." );
			throw std::runtime_error{ "Unsupported WrapMode" };
		}

		return 0;
	}
}

#endif
