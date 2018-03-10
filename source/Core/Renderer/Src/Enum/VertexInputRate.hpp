/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_VertexInputRate_HPP___
#define ___Renderer_VertexInputRate_HPP___
#pragma once

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Vertex layout input rates enumeration.
	*\~french
	*\brief
	*	Enumération des cadences d'entrée pour les layout de tampons de sommets.
	*/
	enum class VertexInputRate
		: int32_t
	{
		eVertex = 0,
		eInstance = 1,
		Utils_EnumBounds( eVertex )
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
	inline std::string getName( VertexInputRate value )
	{
		switch ( value )
		{
		case VertexInputRate::eVertex:
			return "vertex";

		case VertexInputRate::eInstance:
			return "instance";

		default:
			assert( false && "Unsupported VertexInputRate." );
			throw std::runtime_error{ "Unsupported VertexInputRate" };
		}

		return 0;
	}
}

#endif
