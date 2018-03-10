/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_MultisampleState_HPP___
#define ___Renderer_MultisampleState_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~french
	*\brief
	*	Etat de multi-échantillonnage.
	*/
	struct MultisampleState
	{
		MultisampleStateFlags flags = 0;
		SampleCountFlag rasterisationSamples = SampleCountFlag::e1;
		bool sampleShadingEnable = false;
		float minSampleShading = 1.0f;
		uint32_t sampleMask = 0u;
		bool alphaToCoverageEnable = false;
		bool alphaToOneEnable = false;
	};

	inline bool operator==( MultisampleState const & lhs, MultisampleState const & rhs )
	{
		return lhs.flags == rhs.flags
			&& lhs.rasterisationSamples == rhs.rasterisationSamples
			&& lhs.sampleShadingEnable == rhs.sampleShadingEnable
			&& lhs.minSampleShading == rhs.minSampleShading
			&& lhs.alphaToCoverageEnable == rhs.alphaToCoverageEnable
			&& lhs.alphaToOneEnable == rhs.alphaToOneEnable;
	}

	inline bool operator!=( MultisampleState const & lhs, MultisampleState const & rhs )
	{
		return !( lhs == rhs );
	}
}

#endif
