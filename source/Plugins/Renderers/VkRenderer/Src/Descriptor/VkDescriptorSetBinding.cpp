/*
This file belongs to VkLib.
See LICENSE file in root folder.
*/
#include "Descriptor/VkDescriptorSetBinding.hpp"

#include "Buffer/VkBuffer.hpp"
#include "Buffer/VkBufferView.hpp"
#include "Buffer/VkUniformBuffer.hpp"
#include "Descriptor/VkDescriptorSet.hpp"
#include "Descriptor/VkDescriptorSetLayoutBinding.hpp"
#include "Image/VkSampler.hpp"
#include "Image/VkTexture.hpp"
#include "Image/VkTextureView.hpp"

namespace vk_renderer
{
	//************************************************************************************************

	CombinedTextureSamplerBinding::CombinedTextureSamplerBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, DescriptorSet const & descriptorSet
		, TextureView const & view
		, Sampler const & sampler )
		: renderer::CombinedTextureSamplerBinding{ layoutBinding
			, view
			, sampler }
		, m_view{ view }
		, m_sampler{ sampler }
		, m_info
		{
			m_sampler,                                      // sampler
			m_view,                                         // imageView
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL        // imageLayout
		}
	{
		m_write =
		{
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,         // sType
			nullptr,                                        // pNext
			descriptorSet,                                  // dstSet
			layoutBinding.getBindingPoint(),                // dstBinding
			0u,                                             // dstArrayElement
			1u,                                             // descriptorCount
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,      // descriptorType
			&m_info,                                        // pImageInfo
			nullptr,                                        // pBufferInfo
			nullptr                                         // pTexelBufferView
		};
	}

	//************************************************************************************************

	SamplerBinding::SamplerBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, DescriptorSet const & descriptorSet
		, Sampler const & sampler )
		: renderer::SamplerBinding{ layoutBinding
			, sampler }
		, m_sampler{ sampler }
		, m_info
		{
			m_sampler,                                      // sampler
			0,                                              // imageView
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL        // imageLayout
		}
	{
		m_write =
		{
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,         // sType
			nullptr,                                        // pNext
			descriptorSet,                                  // dstSet
			layoutBinding.getBindingPoint(),                // dstBinding
			0u,                                             // dstArrayElement
			1u,                                             // descriptorCount
			VK_DESCRIPTOR_TYPE_SAMPLER,                     // descriptorType
			&m_info,                                        // pImageInfo
			nullptr,                                        // pBufferInfo
			nullptr                                         // pTexelBufferView
		};
	}

	//************************************************************************************************

	SampledTextureBinding::SampledTextureBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, DescriptorSet const & descriptorSet
		, TextureView const & view
		, renderer::ImageLayout layout )
		: renderer::SampledTextureBinding{ layoutBinding, view, layout }
		, m_view{ view }
		, m_info
		{
			VK_NULL_HANDLE,                                 // sampler
			m_view,                                         // imageView
			convert( layout )                               // imageLayout
		}
	{
		m_write =
		{
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,         // sType
			nullptr,                                        // pNext
			descriptorSet,                                  // dstSet
			layoutBinding.getBindingPoint(),                // dstBinding
			0u,                                             // dstArrayElement
			1u,                                             // descriptorCount
			VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,               // descriptorType
			&m_info,                                        // pImageInfo
			nullptr,                                        // pBufferInfo
			nullptr                                         // pTexelBufferView
		};
	}

	//************************************************************************************************

	StorageTextureBinding::StorageTextureBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, DescriptorSet const & descriptorSet
		, TextureView const & view )
		: renderer::StorageTextureBinding{ layoutBinding, view }
		, m_view{ view }
		, m_info
		{
			VK_NULL_HANDLE,                                 // sampler
			m_view,                                         // imageView
			VK_IMAGE_LAYOUT_GENERAL                         // imageLayout
		}
	{
		m_write =
		{
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,         // sType
			nullptr,                                        // pNext
			descriptorSet,                                  // dstSet
			layoutBinding.getBindingPoint(),                // dstBinding
			0u,                                             // dstArrayElement
			1u,                                             // descriptorCount
			VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,               // descriptorType
			&m_info,                                        // pImageInfo
			nullptr,                                        // pBufferInfo
			nullptr                                         // pTexelBufferView
		};
	}

	//************************************************************************************************

	UniformBufferBinding::UniformBufferBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, DescriptorSet const & descriptorSet
		, UniformBuffer const & uniformBuffer
		, uint32_t offset )
		: renderer::UniformBufferBinding{ layoutBinding, uniformBuffer, offset }
		, m_uniformBuffer{ static_cast< Buffer const & >( uniformBuffer.getBuffer() ) }
		, m_info
		{
			m_uniformBuffer,                                // buffer
			offset,                                         // offset
			uniformBuffer.getSize()                         // range
		}
	{
		m_write =
		{
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,         // sType
			nullptr,                                        // pNext
			descriptorSet,                                  // dstSet
			layoutBinding.getBindingPoint(),                // dstBinding
			0u,                                             // dstArrayElement
			1u,                                             // descriptorCount
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,              // descriptorType
			nullptr,                                        // pImageInfo
			&m_info,                                        // pBufferInfo
			nullptr                                         // pTexelBufferView
		};
	}

	//************************************************************************************************

	StorageBufferBinding::StorageBufferBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, DescriptorSet const & descriptorSet
		, Buffer const & storageBuffer
		, uint32_t offset )
		: renderer::StorageBufferBinding{ layoutBinding, storageBuffer, offset }
		, m_buffer{ storageBuffer }
		, m_info
		{
			m_buffer,                                       // buffer
			offset,                                         // offset
			m_buffer.getSize()                              // range
		}
	{
		m_write =
		{
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,         // sType
			nullptr,                                        // pNext
			descriptorSet,                                  // dstSet
			layoutBinding.getBindingPoint(),                // dstBinding
			0u,                                             // dstArrayElement
			1u,                                             // descriptorCount
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,              // descriptorType
			nullptr,                                        // pImageInfo
			&m_info,                                        // pBufferInfo
			nullptr                                         // pTexelBufferView
		};
	}

	//************************************************************************************************

	TexelBufferBinding::TexelBufferBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, DescriptorSet const & descriptorSet
		, Buffer const & buffer
		, BufferView const & view )
		: renderer::TexelBufferBinding{ layoutBinding, buffer, view }
		, m_buffer{ buffer }
		, m_view{ view }
		, m_info
		{
			m_buffer,                                       // buffer
			view.getOffset(),                               // offset
			view.getRange()                                 // range
		}
	{
		auto type = checkFlag( buffer.getTargets(), renderer::BufferTarget::eStorageBuffer )
			? VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER
			: VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
		m_write =
		{
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,         // sType
			nullptr,                                        // pNext
			descriptorSet,                                  // dstSet
			layoutBinding.getBindingPoint(),                // dstBinding
			0u,                                             // dstArrayElement
			1u,                                             // descriptorCount
			type,                                           // descriptorType
			nullptr,                                        // pImageInfo
			&m_info,                                        // pBufferInfo
			&static_cast< VkBufferView const & >( m_view )  // pTexelBufferView
		};
	}

	//************************************************************************************************
}
