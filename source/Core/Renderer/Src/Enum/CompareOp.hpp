/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_CompareOp_HPP___
#define ___Renderer_CompareOp_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Opérateurs de comparaison.
	*/
	enum class CompareOp
		: uint32_t
	{
		eNever,
		eLess,
		eEqual,
		eLessEqual,
		eGreater,
		eNotEqual,
		eGreaterEqual,
		eAlways,
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
	inline std::string getName( CompareOp value )
	{
		switch ( value )
		{
		case CompareOp::eNever:
			return "never";

		case CompareOp::eLess:
			return "less";

		case CompareOp::eEqual:
			return "equal";

		case CompareOp::eLessEqual:
			return "less_equal";

		case CompareOp::eGreater:
			return "greater";

		case CompareOp::eNotEqual:
			return "not_equal";

		case CompareOp::eGreaterEqual:
			return "greater_equal";

		case CompareOp::eAlways:
			return "always";

		default:
			assert( false && "Unsupported CompareOp." );
			throw std::runtime_error{ "Unsupported CompareOp" };
		}

		return 0;
	}
}

#endif
