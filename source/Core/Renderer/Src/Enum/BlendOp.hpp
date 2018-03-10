/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_BlendOp_HPP___
#define ___Renderer_BlendOp_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Opérateurs de mélange.
	*/
	enum class BlendOp
		: uint32_t
	{
		eAdd,
		eSubtract,
		eReverseSubtract,
		eMin,
		eMax,
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
	inline std::string getName( BlendOp value )
	{
		switch ( value )
		{
		case BlendOp::eAdd:
			return "add";

		case BlendOp::eSubtract:
			return "subtract";

		case BlendOp::eReverseSubtract:
			return "rev_subtract";

		case BlendOp::eMin:
			return "min";

		case BlendOp::eMax:
			return "max";

		default:
			assert( false && "Unsupported BlendOp." );
			throw std::runtime_error{ "Unsupported BlendOp" };
		}

		return 0;
	}
}

#endif
