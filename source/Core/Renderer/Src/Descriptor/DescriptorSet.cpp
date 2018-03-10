/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Descriptor/DescriptorSet.hpp"

#include "Buffer/UniformBuffer.hpp"
#include "Descriptor/DescriptorSetLayoutBinding.hpp"
#include "Descriptor/DescriptorSetPool.hpp"

namespace renderer
{
	DescriptorSet::DescriptorSet( DescriptorPool const & pool, uint32_t bindingPoint )
		: m_bindingPoint{ bindingPoint }
		, m_pool{ pool }
	{
	}

	void DescriptorSet::setBindings( WriteDescriptorSetArray bindings )
	{
		m_writes = std::move( bindings );
	}

	WriteDescriptorSet DescriptorSet::createBinding( DescriptorSetLayoutBinding const & layoutBinding
		, TextureView const & view
		, Sampler const & sampler
		, ImageLayout layout
		, uint32_t index )
	{
		m_writes.push_back( 
		{
			layoutBinding.getBindingPoint(),
			index,
			1u,
			DescriptorType::eCombinedImageSampler,
			DescriptorImageInfo{ std::ref( sampler ), std::ref( view ), layout },
			std::nullopt,
			std::nullopt,
		} );
		return m_writes.back();
	}

	WriteDescriptorSet DescriptorSet::createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, renderer::Sampler const & sampler
		, uint32_t index )
	{
		m_writes.push_back(
		{
			layoutBinding.getBindingPoint(),
			index,
			1u,
			DescriptorType::eSampler,
			DescriptorImageInfo{ std::ref( sampler ), std::nullopt, ImageLayout::eShaderReadOnlyOptimal },
			std::nullopt,
			std::nullopt,
		} );
		return m_writes.back();
	}

	WriteDescriptorSet DescriptorSet::createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, renderer::TextureView const & view
		, renderer::ImageLayout layout
		, uint32_t index )
	{
		m_writes.push_back(
		{
			layoutBinding.getBindingPoint(),
			index,
			1u,
			DescriptorType::eSampledImage,
			DescriptorImageInfo{ std::nullopt, std::ref( view ), layout },
			std::nullopt,
			std::nullopt,
		} );
		return m_writes.back();
	}

	WriteDescriptorSet DescriptorSet::createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, renderer::TextureView const & view
		, uint32_t index )
	{
		m_writes.push_back(
		{
			layoutBinding.getBindingPoint(),
			index,
			1u,
			DescriptorType::eStorageImage,
			DescriptorImageInfo{ std::nullopt, std::ref( view ), ImageLayout::eGeneral },
			std::nullopt,
			std::nullopt,
		} );
		return m_writes.back();
	}

	WriteDescriptorSet DescriptorSet::createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, renderer::UniformBufferBase const & uniformBuffer
		, uint32_t offset
		, uint32_t range
		, uint32_t index )
	{
		m_writes.push_back(
		{
			layoutBinding.getBindingPoint(),
			index,
			1u,
			DescriptorType::eUniformBuffer,
			std::nullopt,
			DescriptorBufferInfo{ uniformBuffer.getBuffer(), offset, range },
			std::nullopt,
		} );
		return m_writes.back();
	}

	WriteDescriptorSet DescriptorSet::createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, renderer::BufferBase const & buffer
		, uint32_t offset
		, uint32_t range
		, uint32_t index )
	{
		auto type = checkFlag( buffer.getTargets(), renderer::BufferTarget::eStorageBuffer )
			? DescriptorType::eStorageTexelBuffer
			: DescriptorType::eUniformTexelBuffer;
		m_writes.push_back(
		{
			layoutBinding.getBindingPoint(),
			index,
			1u,
			type,
			std::nullopt,
			DescriptorBufferInfo{ buffer, offset, range },
			std::nullopt,
		} );
		return m_writes.back();
	}

	WriteDescriptorSet DescriptorSet::createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, renderer::BufferBase const & buffer
		, renderer::BufferView const & view
		, uint32_t index )
	{
		auto type = checkFlag( buffer.getTargets(), renderer::BufferTarget::eStorageBuffer )
			? DescriptorType::eStorageTexelBuffer
			: DescriptorType::eUniformTexelBuffer;
		m_writes.push_back(
		{
			layoutBinding.getBindingPoint(),
			index,
			1u,
			type,
			std::nullopt,
			DescriptorBufferInfo{ buffer, view.getOffset(), view.getRange() },
			view,
		} );
		return m_writes.back();
	}

	WriteDescriptorSet DescriptorSet::createDynamicBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, renderer::UniformBufferBase const & uniformBuffer
		, uint32_t offset
		, uint32_t range
		, uint32_t index )
	{
		m_writes.push_back(
		{
			layoutBinding.getBindingPoint(),
			index,
			1u,
			DescriptorType::eUniformBufferDynamic,
			std::nullopt,
			DescriptorBufferInfo{ uniformBuffer.getBuffer(), offset, range },
			std::nullopt,
		} );
		return m_writes.back();
	}

	WriteDescriptorSet DescriptorSet::createDynamicBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, renderer::BufferBase const & buffer
		, uint32_t offset
		, uint32_t range
		, uint32_t index )
	{
		auto type = checkFlag( buffer.getTargets(), renderer::BufferTarget::eStorageBuffer )
			? DescriptorType::eStorageBufferDynamic
			: DescriptorType::eUniformBufferDynamic;
		m_writes.push_back(
		{
			layoutBinding.getBindingPoint(),
			index,
			1u,
			type,
			std::nullopt,
			DescriptorBufferInfo{ buffer, offset, range },
			std::nullopt,
		} );
		return m_writes.back();
	}
}
