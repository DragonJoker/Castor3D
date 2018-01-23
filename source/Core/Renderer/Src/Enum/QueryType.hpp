/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_QueryType_HPP___
#define ___Renderer_QueryType_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Types de requête GPU.
	*/
	enum class QueryType
		: uint32_t
	{
		eOcclusion = 0,
		ePipelineStatistics = 1,
		eTimestamp = 2,
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
	inline std::string getName( QueryType value )
	{
		switch ( value )
		{
		case QueryType::eOcclusion:
			return "occlusion";

		case QueryType::ePipelineStatistics:
			return "statistics";

		case QueryType::eTimestamp:
			return "timestamp";

		default:
			assert( false && "Unsupported QueryType." );
			throw std::runtime_error{ "Unsupported QueryType" };
		}

		return 0;
	}
}

#endif
