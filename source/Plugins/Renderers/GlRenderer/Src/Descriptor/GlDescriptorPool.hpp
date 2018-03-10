/**
*\file
*	DescriptorSetPool.h
*\author
*	Sylvain Doremus
*/
#ifndef ___GlRenderer_DescriptorPool_HPP___
#define ___GlRenderer_DescriptorPool_HPP___
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Descriptor/DescriptorPool.hpp>

namespace gl_renderer
{
	class DescriptorPool
		: public renderer::DescriptorPool
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] layout
		*	Le layout à partir duquel sera créé le pool.
		*/
		DescriptorPool( Device const & device
			, renderer::DescriptorPoolCreateFlags flags
			, uint32_t maxSets
			, renderer::DescriptorPoolSizeArray poolSizes );
		/**
		*\copydoc	renderer::DescriptorSetPool::createDescriptorSet
		*/
		renderer::DescriptorSetPtr createDescriptorSet( renderer::DescriptorSetLayout const & layout
			, uint32_t bindingPoint )const override;
	};
}

#endif
