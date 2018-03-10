/**
*\file
*	VertexBuffer.h
*\author
*	Sylvain Doremus
*/
#ifndef ___GlRenderer_DescriptorSet_HPP___
#define ___GlRenderer_DescriptorSet_HPP___
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Descriptor/DescriptorSet.hpp>

#include <vector>

namespace gl_renderer
{
	/**
	*\brief
	*	Set de descripteurs.
	*/
	class DescriptorSet
		: public renderer::DescriptorSet
	{
	public:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] pool
		*	Le pool parent.
		*\param[in] bindingPoint
		*	Le point d'attache du set.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] pool
		*	The parent pool.
		*\param[in] bindingPoint
		*	The binding point for the set.
		*/
		DescriptorSet( renderer::DescriptorPool const & pool
			, renderer::DescriptorSetLayout const & layout
			, uint32_t bindingPoint );
		/**
		*\copydoc		renderer::DescriptorSet::update
		*/
		void update()const override;
		/**
		*\brief
		*	Le tableau d'attaches de type sampler + texture.
		*/
		inline renderer::WriteDescriptorSetArray const & getCombinedTextureSamplers()const
		{
			return m_combinedTextureSamplers;
		}
		/**
		*\brief
		*	Le tableau d'attaches de type sampler.
		*/
		inline renderer::WriteDescriptorSetArray const & getSamplers()const
		{
			return m_samplers;
		}
		/**
		*\brief
		*	Le tableau d'attaches de type texture échantillonnée.
		*/
		inline renderer::WriteDescriptorSetArray const & getSampledTextures()const
		{
			return m_sampledTextures;
		}
		/**
		*\brief
		*	Le tableau d'attaches de type texture de stockage.
		*/
		inline renderer::WriteDescriptorSetArray const & getStorageTextures()const
		{
			return m_storageTextures;
		}
		/**
		*\brief
		*	Le tableau d'attaches de type tampon uniforme.
		*/
		inline renderer::WriteDescriptorSetArray const & getUniformBuffers()const
		{
			return m_uniformBuffers;
		}
		/**
		*\brief
		*	Le tableau d'attaches de type tampon de stockage.
		*/
		inline renderer::WriteDescriptorSetArray const & getStorageBuffers()const
		{
			return m_storageBuffers;
		}
		/**
		*\brief
		*	Le tableau d'attaches de type tampon uniforme de texels.
		*/
		inline renderer::WriteDescriptorSetArray const & getTexelBuffers()const
		{
			return m_texelBuffers;
		}
		/**
		*\brief
		*	Le tableau d'attaches de type tampon dynamique.
		*/
		inline renderer::WriteDescriptorSetArray const & getDynamicBuffers()const
		{
			return m_dynamicBuffers;
		}

	private:
		mutable renderer::WriteDescriptorSetArray m_combinedTextureSamplers;
		mutable renderer::WriteDescriptorSetArray m_samplers;
		mutable renderer::WriteDescriptorSetArray m_sampledTextures;
		mutable renderer::WriteDescriptorSetArray m_storageTextures;
		mutable renderer::WriteDescriptorSetArray m_uniformBuffers;
		mutable renderer::WriteDescriptorSetArray m_storageBuffers;
		mutable renderer::WriteDescriptorSetArray m_texelBuffers;
		mutable renderer::WriteDescriptorSetArray m_dynamicUniformBuffers;
		mutable renderer::WriteDescriptorSetArray m_dynamicStorageBuffers;
		mutable renderer::WriteDescriptorSetArray m_dynamicBuffers;
	};
}

#endif
