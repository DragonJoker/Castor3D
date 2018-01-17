/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_DescriptorSet_HPP___
#define ___Renderer_DescriptorSet_HPP___
#pragma once

#include "DescriptorSetBinding.hpp"

#include <vector>

namespace renderer
{
	/**
	*\brief
	*	Set de descripteurs.
	*/
	class DescriptorSet
	{
	protected:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*/
		DescriptorSet( DescriptorSetPool const & pool );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~DescriptorSet() = default;
		/**
		*\brief
		*	Crée une attache de type image et échantillonneur combinés.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] view
		*	L'image.
		*\param[in] sampler
		*	L'échantillonneur.
		*\return
		*	L'attache créée.
		*/
		virtual CombinedTextureSamplerBinding const & createBinding( DescriptorSetLayoutBinding const & layoutBinding
			, Texture const & view
			, Sampler const & sampler ) = 0;
		/**
		*\brief
		*	Crée une attache de type image échantillonée.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] view
		*	L'image.
		*\return
		*	L'attache créée.
		*/
		virtual SampledTextureBinding const & createBinding( DescriptorSetLayoutBinding const & layoutBinding
			, Texture const & view ) = 0;
		/**
		*\brief
		*	Crée une attache de type tampon de variables uniformes.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] uniformBuffer
		*	Le tampon.
		*\return
		*	L'attache créée.
		*/
		virtual UniformBufferBinding const & createBinding( DescriptorSetLayoutBinding const & layoutBinding
			, UniformBufferBase const & uniformBuffer
			, uint32_t offset ) = 0;
		/**
		*\brief
		*	Crée une attache de type tampon de variables uniformes.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] uniformBuffer
		*	Le tampon.
		*\return
		*	L'attache créée.
		*/
		template< typename T >
		inline UniformBufferBinding const & createBinding( DescriptorSetLayoutBinding const & layoutBinding
			, UniformBuffer< T > const & uniformBuffer
			, uint32_t offset )
		{
			return createBinding( layoutBinding
				, uniformBuffer.getUbo()
				, offset );
		}
		/**
		*\brief
		*	Met à jour toutes les attaches du descripteur.
		*/
		virtual void update()const = 0;
	};
}

#endif
