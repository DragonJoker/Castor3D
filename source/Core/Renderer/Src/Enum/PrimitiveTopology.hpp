/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_PrimitiveTopology_HPP___
#define ___Renderer_PrimitiveTopology_HPP___
#pragma once

namespace renderer
{
	/**
	*\~french
	*\brief
	*	Enumération des types de primitives.
	*\~english
	*\brief
	*	Primitive types enumeration.
	*/
	enum class PrimitiveTopology
		: uint32_t
	{
		ePointList,
		eLineList,
		eLineStrip,
		eTriangleList,
		eTriangleStrip,
		eTriangleFan,
		eLineListWithAdjacency,
		eLineStripWithAdjacency,
		eTriangleListWithAdjacency,
		eTriangleStripWithAdjacency,
		ePatchList,
		Utils_EnumBounds( ePointList )
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
	inline std::string getName( PrimitiveTopology value )
	{
		switch ( value )
		{
		case PrimitiveTopology::ePointList:
			return "point_list";

		case PrimitiveTopology::eLineList:
			return "line_list";

		case PrimitiveTopology::eLineStrip:
			return "line_strip";

		case PrimitiveTopology::eTriangleList:
			return "triangle_list";

		case PrimitiveTopology::eTriangleStrip:
			return "triangle_strip";

		case PrimitiveTopology::eTriangleFan:
			return "triangle_fan";

		case PrimitiveTopology::eLineListWithAdjacency:
			return "line_list_adj";

		case PrimitiveTopology::eLineStripWithAdjacency:
			return "line_strip_adj";

		case PrimitiveTopology::eTriangleListWithAdjacency:
			return "triangle_list_adj";

		case PrimitiveTopology::eTriangleStripWithAdjacency:
			return "triangle_strip_adj";

		case PrimitiveTopology::ePatchList:
			return "path_list";

		default:
			assert( false && "Unsupported PrimitiveTopology." );
			throw std::runtime_error{ "Unsupported PrimitiveTopology" };
		}

		return 0;
	}
}

#endif
