/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_BlendFactor_HPP___
#define ___Renderer_BlendFactor_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Les facteurs de mélange.
	*/
	enum class BlendFactor
		: uint32_t
	{
		eZero,
		eOne,
		eSrcColour,
		eInvSrcColour,
		eDstColour,
		eInvDstColour,
		eSrcAlpha,
		eInvSrcAlpha,
		eDstAlpha,
		eInvDstAlpha,
		eConstantColour,
		eInvConstantColour,
		eConstantAlpha,
		eInvConstantAlpha,
		eSrcAlphaSaturate,
		eSrc1Colour,
		eInvSrc1Colour,
		eSrc1Alpha,
		eInvSrc1Alpha,
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
	inline std::string getName( BlendFactor value )
	{
		switch ( value )
		{
		case BlendFactor::eZero:
			return "zero";

		case BlendFactor::eOne:
			return "one";

		case BlendFactor::eSrcColour:
			return "src_colour";

		case BlendFactor::eInvSrcColour:
			return "inv_src_colour";

		case BlendFactor::eDstColour:
			return "dst_colour";

		case BlendFactor::eInvDstColour:
			return "inv_dst_colour";

		case BlendFactor::eSrcAlpha:
			return "src_alpha";

		case BlendFactor::eInvSrcAlpha:
			return "inv_src_alpha";

		case BlendFactor::eDstAlpha:
			return "dst_alpha";

		case BlendFactor::eInvDstAlpha:
			return "inv_dst_alpha";

		case BlendFactor::eConstantColour:
			return "constant_colour";

		case BlendFactor::eInvConstantColour:
			return "inv_constant_colour";

		case BlendFactor::eConstantAlpha:
			return "constant_alpha";

		case BlendFactor::eInvConstantAlpha:
			return "inv_constant_alpha";

		case BlendFactor::eSrcAlphaSaturate:
			return "src_alpha_saturate";

		case BlendFactor::eSrc1Colour:
			return "src1_colour";

		case BlendFactor::eInvSrc1Colour:
			return "inv_src1_colour";

		case BlendFactor::eSrc1Alpha:
			return "src1_alpha";

		case BlendFactor::eInvSrc1Alpha:
			return "inv_src1_alpha";

		default:
			assert( false && "Unsupported BlendFactor." );
			throw std::runtime_error{ "Unsupported BlendFactor" };
		}

		return 0;
	}
}

#endif
