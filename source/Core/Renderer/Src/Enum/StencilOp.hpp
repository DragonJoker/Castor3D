/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_StencilOp_HPP___
#define ___Renderer_StencilOp_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Opérateurs logiques.
	*/
	enum class StencilOp
		: uint32_t
	{
		eKeep,
		eZero,
		eReplace,
		eIncrementAndClamp,
		eDecrementAndClamp,
		eInvert,
		eIncrementAndWrap,
		eDecrementAndWrap,
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
	inline std::string getName( StencilOp value )
	{
		switch ( value )
		{
		case StencilOp::eKeep:
			return "keep";

		case StencilOp::eZero:
			return "zero";

		case StencilOp::eReplace:
			return "replace";

		case StencilOp::eIncrementAndClamp:
			return "incr_clamp";

		case StencilOp::eDecrementAndClamp:
			return "decr_clamp";

		case StencilOp::eInvert:
			return "invert";

		case StencilOp::eIncrementAndWrap:
			return "incr_wrap";

		case StencilOp::eDecrementAndWrap:
			return "decr_wrap";

		default:
			assert( false && "Unsupported StencilOp." );
			throw std::runtime_error{ "Unsupported StencilOp" };
		}

		return 0;
	}
}

#endif
