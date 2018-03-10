/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_TextureViewType_HPP___
#define ___Renderer_TextureViewType_HPP___
#pragma once

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Texture view types enumeration.
	*\~french
	*\brief
	*	Enumération des types de vue sur une texture.
	*/
	enum class TextureViewType
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
	inline std::string getName( TextureViewType value )
	{
		switch ( value )
		{
		case TextureViewType::e1D:
			return "1d";

		case TextureViewType::e2D:
			return "2d";

		case TextureViewType::e3D:
			return "3d";

		case TextureViewType::eCube:
			return "cube";

		case TextureViewType::e1DArray:
			return "1d_array";

		case TextureViewType::e2DArray:
			return "2d_array";

		case TextureViewType::eCubeArray:
			return "cube_array";

		default:
			assert( false && "Unsupported TextureViewType." );
			throw std::runtime_error{ "Unsupported TextureViewType" };
		}

		return 0;
	}
}

#endif
