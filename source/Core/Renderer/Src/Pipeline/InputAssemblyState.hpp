/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_InputAssemblyState_HPP___
#define ___Renderer_InputAssemblyState_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies parameters of a pipeline input assembly state.
	*\~french
	*\brief
	*	Définit les paramètres d'un état d'assemblage des entrées.
	*/
	struct InputAssemblyState
	{
		PrimitiveTopology topology = PrimitiveTopology::eTriangleList;
		bool primitiveRestartEnable = false;
	};

	inline bool operator==( InputAssemblyState const & lhs, InputAssemblyState const & rhs )
	{
		return lhs.topology == rhs.topology
			&& lhs.primitiveRestartEnable == rhs.primitiveRestartEnable;
	}

	inline bool operator!=( InputAssemblyState const & lhs, InputAssemblyState const & rhs )
	{
		return !( lhs == rhs );
	}
}

#endif
