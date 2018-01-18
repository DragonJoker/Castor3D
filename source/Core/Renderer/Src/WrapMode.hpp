/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_WrapMode_HPP___
#define ___Renderer_WrapMode_HPP___
#pragma once

#include <CastorUtilsPrerequisites.hpp>

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
		CASTOR_SCOPED_ENUM_BOUNDS( eRepeat )
	};
	/**
	*\~english
	*\brief
	*	Gets the name of the given renderer::WrapMode.
	*\param[in] value
	*	The renderer::WrapMode.
	*\return
	*	The name.
	*\~french
	*\brief
	*	Récupère le nom du renderer::WrapMode donné.
	*\param[in] value
	*	Le renderer::WrapMode.
	*\return
	*	Le nom.
	*/
	inline std::string const getName( renderer::WrapMode value )
	{
		switch ( value )
		{
		case renderer::WrapMode::eRepeat:
			return "repeat";

		case renderer::WrapMode::eMirroredRepeat:
			return "mirrored_repeat";

		case renderer::WrapMode::eClampToEdge:
			return "clamp_to_edge";

		case renderer::WrapMode::eClampToBorder:
			return "clamp_to_border";

		case renderer::WrapMode::eMirrorClampToEdge:
			return "mirrored_clamp_to_edge";

		default:
			assert( false && "Unsupported wrap mode" );
			throw std::runtime_error{ "Unsupported wrap mode" };
		}
	}
}

#endif
