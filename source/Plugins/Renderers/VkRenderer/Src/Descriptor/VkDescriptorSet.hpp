/**
*\file
*	VertexBuffer.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_DescriptorSet_HPP___
#define ___VkRenderer_DescriptorSet_HPP___
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Descriptor/DescriptorSet.hpp>

#include <vector>

namespace vk_renderer
{
	/**
	*\brief
	*	Classe wrappant un VkDescriptorSet.
	*/
	class DescriptorSet
		: public renderer::DescriptorSet
	{
	public:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] pool
		*	Le pool parent.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] pool
		*	The parent pool.
		*/
		DescriptorSet( Device const & device
			, DescriptorSetPool const & pool );
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		~DescriptorSet();
		/**
		*\copydoc	renderer::createBinding
		*/
		renderer::CombinedTextureSamplerBinding const & createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
			, renderer::TextureView const & view
			, renderer::Sampler const & sampler )override;
		/**
		*\copydoc	renderer::createBinding
		*/
		renderer::SamplerBinding const & createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
			, renderer::Sampler const & sampler )override;
		/**
		*\copydoc	renderer::createBinding
		*/
		renderer::SampledTextureBinding const & createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
			, renderer::TextureView const & view
			, renderer::ImageLayout layout )override;
		/**
		*\copydoc	renderer::createBinding
		*/
		renderer::StorageTextureBinding const & createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
			, renderer::TextureView const & view )override;
		/**
		*\copydoc	renderer::createBinding
		*/
		renderer::UniformBufferBinding const & createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
			, renderer::UniformBufferBase const & uniformBuffer
			, uint32_t offset )override;
		/**
		*\copydoc	renderer::createBinding
		*/
		renderer::StorageBufferBinding const & createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
			, renderer::BufferBase const & storageBuffer
			, uint32_t offset )override;
		/**
		*\copydoc	renderer::createBinding
		*/
		renderer::TexelBufferBinding const & createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
			, renderer::BufferBase const & buffer
			, renderer::BufferView const & view )override;
		/**
		*\~french
		*\brief
		*	Met à jour toutes les attaches du descripteur.
		*\~english
		*\brief
		*	Updates all this descriptor's attaches.
		*/
		void update()const override;
		/**
		*\~french
		*\brief
		*	Conversion implicite vers VkDescriptorSet.
		*\~english
		*\brief
		*	VkDescriptorSet implicit cast operator.
		*/
		inline operator VkDescriptorSet const &( )const
		{
			return m_descriptorSet;
		}

	private:
		Device const & m_device;
		DescriptorSetPool const & m_pool;
		DescriptorSetLayout const & m_layout;
		VkDescriptorSet m_descriptorSet{};
		std::vector< DescriptorSetBindingPtr > m_bindings;
	};
}

#endif
