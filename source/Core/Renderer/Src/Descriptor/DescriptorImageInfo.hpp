/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_DescriptorImageInfo_HPP___
#define ___Renderer_DescriptorImageInfo_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	struct DescriptorImageInfo
	{
		std::optional< SamplerCRef > sampler;
		std::optional< TextureViewCRef > imageView;
		ImageLayout imageLayout;
	};
}

#endif
