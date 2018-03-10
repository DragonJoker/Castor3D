/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_FrontFace_HPP___
#define ___Renderer_FrontFace_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Sens dans lequel les faces sont considérées comme faisant face à la caméra.
	*/
	enum class FrontFace
		: uint32_t
	{
		eCounterClockwise,
		eClockwise,
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
	inline std::string getName( FrontFace value )
	{
		switch ( value )
		{
		case FrontFace::eCounterClockwise:
			return "ccw";

		case FrontFace::eClockwise:
			return "cw";

		default:
			assert( false && "Unsupported FrontFace." );
			throw std::runtime_error{ "Unsupported FrontFace" };
		}

		return 0;
	}
}

#endif
