/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_CullModeFlag_HPP___
#define ___Renderer_CullModeFlag_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Enumération des types d'accès.
	*/
	enum class CullModeFlag
		: uint32_t
	{
		eNone = 0x00000000,
		eFront = 0x00000001,
		eBack = 0x00000002,
		eFrontAndBack = 0x00000003,
	};
	Utils_ImplementFlag( CullModeFlag )
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
	inline std::string getName( CullModeFlag value )
	{
		switch ( value )
		{
		case CullModeFlag::eNone:
			return "none";

		case CullModeFlag::eFront:
			return "front";

		case CullModeFlag::eBack:
			return "back";

		case CullModeFlag::eFrontAndBack:
			return "both";

		default:
			assert( false && "Unsupported CullModeFlag." );
			throw std::runtime_error{ "Unsupported CullModeFlag" };
		}

		return 0;
	}
}

#endif
