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
		DescriptorSet( renderer::DescriptorSetPool const & pool, uint32_t bindingPoint );
		/**
		*\copydoc		renderer::DescriptorSet::createBinding
		*/
		renderer::CombinedTextureSamplerBinding const & createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
			, renderer::TextureView const & view
			, renderer::Sampler const & sampler
			, uint32_t index )override;
		/**
		*\copydoc		renderer::DescriptorSet::createBinding
		*/
		renderer::SamplerBinding const & createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
			, renderer::Sampler const & sampler
			, uint32_t index )override;
		/**
		*\copydoc		renderer::DescriptorSet::createBinding
		*/
		renderer::SampledTextureBinding const & createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
			, renderer::TextureView const & view
			, renderer::ImageLayout layout
			, uint32_t index )override;
		/**
		*\copydoc		renderer::DescriptorSet::createBinding
		*/
		renderer::StorageTextureBinding const & createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
			, renderer::TextureView const & view
			, uint32_t index )override;
		/**
		*\copydoc		renderer::DescriptorSet::createBinding
		*/
		renderer::UniformBufferBinding const & createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
			, renderer::UniformBufferBase const & uniformBuffer
			, uint32_t offset
			, uint32_t range
			, uint32_t index )override;
		/**
		*\copydoc		renderer::DescriptorSet::createBinding
		*/
		renderer::StorageBufferBinding const & createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
			, renderer::BufferBase const & storageBuffer
			, uint32_t offset
			, uint32_t range
			, uint32_t index )override;
		/**
		*\copydoc		renderer::DescriptorSet::createBinding
		*/
		renderer::TexelBufferBinding const & createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
			, renderer::BufferBase const & buffer
			, renderer::BufferView const & view
			, uint32_t index )override;
		/**
		*\copydoc		renderer::DescriptorSet::update
		*/
		void update()const override;
		/**
		*\brief
		*	Le tableau d'attaches de type sampler + texture.
		*/
		inline std::vector< renderer::CombinedTextureSamplerBinding > const & getCombinedTextureSamplers()const
		{
			return m_combinedTextureSamplers;
		}
		/**
		*\brief
		*	Le tableau d'attaches de type sampler.
		*/
		inline std::vector< renderer::SamplerBinding > const & getSamplers()const
		{
			return m_samplers;
		}
		/**
		*\brief
		*	Le tableau d'attaches de type texture échantillonnée.
		*/
		inline std::vector< renderer::SampledTextureBinding > const & getSampledTextures()const
		{
			return m_sampledTextures;
		}
		/**
		*\brief
		*	Le tableau d'attaches de type texture de stockage.
		*/
		inline std::vector< renderer::StorageTextureBinding > const & getStorageTextures()const
		{
			return m_storageTextures;
		}
		/**
		*\brief
		*	Le tableau d'attaches de type tampon uniforme.
		*/
		inline std::vector< renderer::UniformBufferBinding > const & getUniformBuffers()const
		{
			return m_uniformBuffers;
		}
		/**
		*\brief
		*	Le tableau d'attaches de type tampon de stockage.
		*/
		inline std::vector< renderer::StorageBufferBinding > const & getStorageBuffers()const
		{
			return m_storageBuffers;
		}
		/**
		*\brief
		*	Le tableau d'attaches de type tampon uniforme de texels.
		*/
		inline std::vector< renderer::TexelBufferBinding > const & getTexelBuffers()const
		{
			return m_texelBuffers;
		}

	private:
		std::vector< renderer::CombinedTextureSamplerBinding > m_combinedTextureSamplers;
		std::vector< renderer::SamplerBinding > m_samplers;
		std::vector< renderer::SampledTextureBinding > m_sampledTextures;
		std::vector< renderer::StorageTextureBinding > m_storageTextures;
		std::vector< renderer::UniformBufferBinding > m_uniformBuffers;
		std::vector< renderer::StorageBufferBinding > m_storageBuffers;
		std::vector< renderer::TexelBufferBinding > m_texelBuffers;
	};
}

#endif
