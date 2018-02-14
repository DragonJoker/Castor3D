/**
*\file
*	DescriptorSetPool.h
*\author
*	Sylvain Doremus
*/
#ifndef ___GlRenderer_DescriptorSetPool_HPP___
#define ___GlRenderer_DescriptorSetPool_HPP___
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Descriptor/DescriptorSetPool.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Pool de sets de descripteurs.
	*/
	class DescriptorSetPool
		: public renderer::DescriptorSetPool
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] layout
		*	Le layout à partir duquel sera créé le pool.
		*/
		DescriptorSetPool( renderer::DescriptorSetLayout const & layout
			, uint32_t maxSets
			, bool automaticFree );
		/**
		*\copydoc	renderer::DescriptorSetPool::createDescriptorSet
		*/
		renderer::DescriptorSetPtr createDescriptorSet( uint32_t bindingPoint )const override;
	};
}

#endif
