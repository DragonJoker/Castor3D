/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_TextureType_HPP___
#define ___Renderer_TextureType_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Enumération des types de texture.
	*/
	enum class TextureType
		: uint32_t
	{
		e1D = 0,
		e2D = 1,
		e3D = 2,
		eCube = 3,
		e1DArray = 4,
		e2DArray = 5,
		eCubeArray = 6,
		Utils_EnumBounds( e1D )
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
	inline std::string getName( TextureType value )
	{
		switch ( value )
		{
		case TextureType::e1D:
			return "1d";

		case TextureType::e2D:
			return "2d";

		case TextureType::e3D:
			return "3d";

		case TextureType::eCube:
			return "cube";

		case TextureType::e1DArray:
			return "1d_array";

		case TextureType::e2DArray:
			return "2d_array";

		case TextureType::eCubeArray:
			return "cube_array";

		default:
			assert( false && "Unsupported TextureType." );
			throw std::runtime_error{ "Unsupported TextureType" };
		}

		return 0;
	}
}

#endif
