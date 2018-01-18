/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_PrimitiveTopology_HPP___
#define ___Renderer_PrimitiveTopology_HPP___
#pragma once

#include <CastorUtilsPrerequisites.hpp>

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
		CASTOR_SCOPED_ENUM_BOUNDS( ePointList )
	};
	/**
	*\~english
	*\brief
	*	Gets the name of the given renderer::PrimitiveTopology.
	*\param[in] value
	*	The renderer::PrimitiveTopology.
	*\return
	*	The name.
	*\~french
	*\brief
	*	Récupère le nom du renderer::PrimitiveTopology donné.
	*\param[in] value
	*	Le renderer::PrimitiveTopology.
	*\return
	*	Le nom.
	*/
	inline std::string const getName( renderer::PrimitiveTopology value )
	{
		switch ( value )
		{
		case renderer::PrimitiveTopology::ePointList:
			return "point_list";

		case renderer::PrimitiveTopology::eLineList:
			return "line_list";

		case renderer::PrimitiveTopology::eLineListWithAdjacency:
			return "line_list_adjacency";

		case renderer::PrimitiveTopology::eLineStrip:
			return "line_strip";

		case renderer::PrimitiveTopology::eLineStripWithAdjacency:
			return "line_strip_adjacency";

		case renderer::PrimitiveTopology::eTriangleList:
			return "triangle_list";

		case renderer::PrimitiveTopology::eTriangleListWithAdjacency:
			return "triangle_list_adjacency";

		case renderer::PrimitiveTopology::eTriangleStrip:
			return "triangle_strip";

		case renderer::PrimitiveTopology::eTriangleStripWithAdjacency:
			return "triangle_strip_adjacency";

		case renderer::PrimitiveTopology::eTriangleFan:
			return "triangle_fan";

		case renderer::PrimitiveTopology::ePatchList:
			return "patch_list";

		default:
			assert( false && "Topology type unknown" );
			throw std::runtime_error{ "Topology type unknown" };
		}
	}
}

#endif
