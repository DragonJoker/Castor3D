/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_CompareOp_HPP___
#define ___Renderer_CompareOp_HPP___
#pragma once

#include <Design/FlagCombination.hpp>

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
	*	Gets the name of the given renderer::CompareOp.
	*\param[in] value
	*	The renderer::CompareOp.
	*\return
	*	The name.
	*\~french
	*\brief
	*	Récupère le nom du renderer::CompareOp donné.
	*\param[in] value
	*	Le renderer::CompareOp.
	*\return
	*	Le nom.
	*/
	inline std::string const getName( renderer::CompareOp value )
	{
		switch ( value )
		{
		case renderer::CompareOp::eNever:
			return "never";

		case renderer::CompareOp::eLess:
			return "less";

		case renderer::CompareOp::eEqual:
			return "equal";

		case renderer::CompareOp::eLessEqual:
			return "less_or_equal";

		case renderer::CompareOp::eGreater:
			return "greater";

		case renderer::CompareOp::eNotEqual:
			return "not_equal";

		case renderer::CompareOp::eGreaterEqual:
			return "greater_or_equal";

		case renderer::CompareOp::eAlways:
			return "always";

		default:
			assert( false && "Unsupported renderer::CompareOp" );
			throw std::runtime_error{ "Unsupported renderer::CompareOp" };
		}
	}
}

#endif
