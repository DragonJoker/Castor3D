/**
*\file
*	VertexBuffer.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_DescriptorSetLayout_HPP___
#define ___VkRenderer_DescriptorSetLayout_HPP___
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Descriptor/DescriptorSetLayout.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Layout de descripteurs.
	*/
	class DescriptorSetLayout
		: public renderer::DescriptorSetLayout
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*/
		DescriptorSetLayout( renderer::Device const & device
			, renderer::DescriptorSetLayoutBindingArray && bindings );
		/**
		*\brief
		*	Crée un pool pour les descripteurs qui utiliseront ce layout.
		*\param[in] maxSets
		*	Le nombre maximum de sets que le pool peut créer.
		*\return
		*	Le pool.
		*/
		renderer::DescriptorSetPoolPtr createPool( uint32_t maxSets )const override;
	};
}

#endif
