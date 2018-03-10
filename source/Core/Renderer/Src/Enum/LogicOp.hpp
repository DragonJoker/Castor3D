/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_LogicOp_HPP___
#define ___Renderer_LogicOp_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Opérateurs logiques.
	*/
	enum class LogicOp
		: uint32_t
	{
		eClear,
		eAnd,
		eAndReverse,
		eCopy,
		eAndInverted,
		eNoOp,
		eXor,
		eOr,
		eNor,
		eEquivalent,
		eInvert,
		eOrReverse,
		eCopyInverted,
		eOrInverted,
		eNand,
		eSet,
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
	inline std::string getName( LogicOp value )
	{
		switch ( value )
		{
		case LogicOp::eClear:
			return "clear";

		case LogicOp::eAnd:
			return "and";

		case LogicOp::eAndReverse:
			return "and_reverse";

		case LogicOp::eCopy:
			return "copy";

		case LogicOp::eAndInverted:
			return "and_inverted";

		case LogicOp::eNoOp:
			return "noop";

		case LogicOp::eXor:
			return "xor";

		case LogicOp::eOr:
			return "or";

		case LogicOp::eNor:
			return "nor";

		case LogicOp::eEquivalent:
			return "equivalent";

		case LogicOp::eInvert:
			return "invert";

		case LogicOp::eOrReverse:
			return "or_reverse";

		case LogicOp::eCopyInverted:
			return "copy_inverted";

		case LogicOp::eOrInverted:
			return "or_inverted";

		case LogicOp::eNand:
			return "nand";

		case LogicOp::eSet:
			return "set";

		default:
			assert( false && "Unsupported LogicOp." );
			throw std::runtime_error{ "Unsupported LogicOp" };
		}

		return 0;
	}
}

#endif
