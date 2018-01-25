/**
*\file
*	DescriptorSetPool.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_DescriptorSetPool_HPP___
#define ___VkRenderer_DescriptorSetPool_HPP___
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
			, uint32_t maxSets );
		/**
		*\brief
		*	Crée un descriptor set correspondant au layout défini pour ce pool.
		*\return
		*	Le descriptor set créé.
		*/
		renderer::DescriptorSetPtr createDescriptorSet()const override;
	};
}

#endif
