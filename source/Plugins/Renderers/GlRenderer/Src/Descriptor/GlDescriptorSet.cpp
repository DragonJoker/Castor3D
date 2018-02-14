#include "Descriptor/GlDescriptorSet.hpp"

#include "Buffer/GlBuffer.hpp"
#include "Descriptor/GlDescriptorSetPool.hpp"
#include "Image/GlSampler.hpp"
#include "Image/GlTexture.hpp"
#include "Buffer/GlUniformBuffer.hpp"

namespace gl_renderer
{
	DescriptorSet::DescriptorSet( renderer::DescriptorSetPool const & pool, uint32_t bindingPoint )
		: renderer::DescriptorSet{ pool, bindingPoint }
	{
	}

	renderer::CombinedTextureSamplerBinding const & DescriptorSet::createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, renderer::TextureView const & view
		, renderer::Sampler const & sampler
		, uint32_t index )
	{
		m_combinedTextureSamplers.emplace_back( layoutBinding
			, view
			, sampler
			, index );
		return m_combinedTextureSamplers.back();
	}

	renderer::SamplerBinding const & DescriptorSet::createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, renderer::Sampler const & sampler
		, uint32_t index )
	{
		m_samplers.emplace_back( layoutBinding
			, sampler
			, index );
		return m_samplers.back();
	}

	renderer::SampledTextureBinding const & DescriptorSet::createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, renderer::TextureView const & view
		, renderer::ImageLayout layout
		, uint32_t index )
	{
		m_sampledTextures.emplace_back( layoutBinding
			, view
			, layout
			, index );
		return m_sampledTextures.back();
	}

	renderer::StorageTextureBinding const & DescriptorSet::createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, renderer::TextureView const & view
		, uint32_t index )
	{
		m_storageTextures.emplace_back( layoutBinding
			, view
			, index );
		return m_storageTextures.back();
	}

	renderer::UniformBufferBinding const & DescriptorSet::createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, renderer::UniformBufferBase const & uniformBuffer
		, uint32_t offset
		, uint32_t range
		, uint32_t index )
	{
		m_uniformBuffers.emplace_back( layoutBinding
			, uniformBuffer
			, offset
			, range
			, index );
		return m_uniformBuffers.back();
	}

	renderer::StorageBufferBinding const & DescriptorSet::createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, renderer::BufferBase const & storageBuffer
		, uint32_t offset
		, uint32_t range
		, uint32_t index )
	{
		m_storageBuffers.emplace_back( layoutBinding
			, storageBuffer
			, offset
			, range
			, index );
		return m_storageBuffers.back();
	}

	renderer::TexelBufferBinding const & DescriptorSet::createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, renderer::BufferBase const & buffer
		, renderer::BufferView const & view
		, uint32_t index )
	{
		m_texelBuffers.emplace_back( layoutBinding
			, buffer
			, view
			, index );
		return m_texelBuffers.back();
	}

	void DescriptorSet::update()const
	{
	}
}
