/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_RasterisationState_HPP___
#define ___Renderer_RasterisationState_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\brief
	*	Etat de rastérisation.
	*/
	struct RasterisationState
	{
		RasterisationStateFlags flags = 0u;
		bool depthClampEnable = false;
		bool rasteriserDiscardEnable = false;
		PolygonMode polygonMode = PolygonMode::eFill;
		CullModeFlags cullMode = CullModeFlag::eBack;
		FrontFace frontFace = FrontFace::eCounterClockwise;
		bool depthBiasEnable = false;
		float depthBiasConstantFactor = 0.0f;
		float depthBiasClamp = 0.0f;
		float depthBiasSlopeFactor = 0.0f;
		float lineWidth = 1.0f;
	};

	inline bool operator==( RasterisationState const & lhs, RasterisationState const & rhs )
	{
		return lhs.flags == rhs.flags
			&& lhs.depthClampEnable == rhs.depthClampEnable
			&& lhs.rasteriserDiscardEnable == rhs.rasteriserDiscardEnable
			&& lhs.polygonMode == rhs.polygonMode
			&& lhs.cullMode == rhs.cullMode
			&& lhs.frontFace == rhs.frontFace
			&& lhs.depthBiasEnable == rhs.depthBiasEnable
			&& lhs.depthBiasConstantFactor == rhs.depthBiasConstantFactor
			&& lhs.depthBiasClamp == rhs.depthBiasClamp
			&& lhs.depthBiasSlopeFactor == rhs.depthBiasSlopeFactor
			&& lhs.lineWidth == rhs.lineWidth;
	}

	inline bool operator!=( RasterisationState const & lhs, RasterisationState const & rhs )
	{
		return !( lhs == rhs );
	}
}

#endif
