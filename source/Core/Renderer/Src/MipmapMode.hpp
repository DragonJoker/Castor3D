/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_MipmapMode_HPP___
#define ___Renderer_MipmapMode_HPP___
#pragma once

#include <CastorUtilsPrerequisites.hpp>

namespace renderer
{
	/**
	*\brief
	*	Enumération des modes de mipmap.
	*/
	enum class MipmapMode
	{
		eNone,
		eNearest,
		eLinear,
		CASTOR_SCOPED_ENUM_BOUNDS( eNearest )
	};
	/**
	*\~english
	*\brief
	*	Gets the name of the given renderer::MipmapMode.
	*\param[in] value
	*	The renderer::MipmapMode.
	*\return
	*	The name.
	*\~french
	*\brief
	*	Récupère le nom du renderer::MipmapMode donné.
	*\param[in] value
	*	Le renderer::MipmapMode.
	*\return
	*	Le nom.
	*/
	inline std::string const getName( renderer::MipmapMode value )
	{
		switch ( value )
		{
		case renderer::MipmapMode::eNone:
			return "none";

		case renderer::MipmapMode::eNearest:
			return "nearest";

		case renderer::MipmapMode::eLinear:
			return "linear";

		default:
			assert( false && "Unsupported renderer::MipmapMode" );
			throw std::runtime_error{ "Unsupported renderer::MipmapMode" };
		}
	}
}

#endif
