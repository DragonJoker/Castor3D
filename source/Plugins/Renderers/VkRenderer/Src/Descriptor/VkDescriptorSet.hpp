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
		*\param[in] bindingPoint
		*	Le point d'attache du set.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] pool
		*	The parent pool.
		*\param[in] bindingPoint
		*	The binding point for the set.
		*/
		DescriptorSet( Device const & device
			, DescriptorSetPool const & pool
			, uint32_t bindingPoint );
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
		*\copydoc	renderer::DescriptorSet::createBinding
		*/
		renderer::CombinedTextureSamplerBinding const & createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
			, renderer::TextureView const & view
			, renderer::Sampler const & sampler
			, uint32_t index )override;
		/**
		*\copydoc	renderer::DescriptorSet::createBinding
		*/
		renderer::SamplerBinding const & createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
			, renderer::Sampler const & sampler
			, uint32_t index )override;
		/**
		*\copydoc	renderer::DescriptorSet::createBinding
		*/
		renderer::SampledTextureBinding const & createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
			, renderer::TextureView const & view
			, renderer::ImageLayout layout
			, uint32_t index )override;
		/**
		*\copydoc	renderer::DescriptorSet::createBinding
		*/
		renderer::StorageTextureBinding const & createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
			, renderer::TextureView const & view
			, uint32_t index )override;
		/**
		*\copydoc	renderer::DescriptorSet::createBinding
		*/
		renderer::UniformBufferBinding const & createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
			, renderer::UniformBufferBase const & uniformBuffer
			, uint32_t offset
			, uint32_t range
			, uint32_t index )override;
		/**
		*\copydoc	renderer::DescriptorSet::createBinding
		*/
		renderer::StorageBufferBinding const & createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
			, renderer::BufferBase const & storageBuffer
			, uint32_t offset
			, uint32_t range
			, uint32_t index )override;
		/**
		*\copydoc	renderer::DescriptorSet::createBinding
		*/
		renderer::TexelBufferBinding const & createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
			, renderer::BufferBase const & buffer
			, renderer::BufferView const & view
			, uint32_t index )override;
		/**
		*\copydoc	renderer::DescriptorSet::update
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
