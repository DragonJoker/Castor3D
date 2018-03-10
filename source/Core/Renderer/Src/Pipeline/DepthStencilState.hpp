/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_DepthStencilState_HPP___
#define ___Renderer_DepthStencilState_HPP___
#pragma once

#include "Pipeline/StencilOpState.hpp"

namespace renderer
{
	/**
	*\english
	*\brief
	*	The depth and stencil state.
	*\french
	*\brief
	*	L'état de stencil et profondeur.
	*/
	struct DepthStencilState
	{
		DepthStencilStateFlags flags = DepthStencilStateFlags{};
		bool depthTestEnable = true;
		bool depthWriteEnable = true;
		CompareOp depthCompareOp = CompareOp::eLess;
		bool depthBoundsTestEnable = false;
		bool stencilTestEnable = false;
		StencilOpState front = StencilOpState{};
		StencilOpState back = StencilOpState{};
		float minDepthBounds = 0.0f;
		float maxDepthBounds = 1.0f;
	};

	inline bool operator==( DepthStencilState const & lhs, DepthStencilState const & rhs )
	{
		return lhs.flags == rhs.flags
			&& lhs.depthTestEnable == rhs.depthTestEnable
			&& lhs.depthWriteEnable == rhs.depthWriteEnable
			&& lhs.depthCompareOp == rhs.depthCompareOp
			&& lhs.depthBoundsTestEnable == rhs.depthBoundsTestEnable
			&& lhs.stencilTestEnable == rhs.stencilTestEnable
			&& lhs.front == rhs.front
			&& lhs.back == rhs.back
			&& lhs.minDepthBounds == rhs.minDepthBounds
			&& lhs.maxDepthBounds == rhs.maxDepthBounds;
	}

	inline bool operator!=( DepthStencilState const & lhs, DepthStencilState const & rhs )
	{
		return !( lhs == rhs );
	}
}

#endif
