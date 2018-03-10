/**
*\file
*	VertexBuffer.h
*\author
*	Sylvain Doremus
*/
#ifndef ___GlRenderer_DescriptorSetLayout_HPP___
#define ___GlRenderer_DescriptorSetLayout_HPP___
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Descriptor/DescriptorSetLayout.hpp>

namespace gl_renderer
{
	class DescriptorSetLayout
		: public renderer::DescriptorSetLayout
	{
	public:
		DescriptorSetLayout( renderer::Device const & device
			, renderer::DescriptorSetLayoutBindingArray && bindings );
	};
}

#endif
