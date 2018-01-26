#include "Descriptor/GlDescriptorSet.hpp"

#include "Buffer/GlBuffer.hpp"
#include "Descriptor/GlDescriptorSetPool.hpp"
#include "Image/GlSampler.hpp"
#include "Image/GlTexture.hpp"
#include "Buffer/GlUniformBuffer.hpp"

namespace gl_renderer
{
	DescriptorSet::DescriptorSet( renderer::DescriptorSetPool const & pool )
		: renderer::DescriptorSet{ pool }
	{
	}

	renderer::CombinedTextureSamplerBinding const & DescriptorSet::createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, renderer::TextureView const & view
		, renderer::Sampler const & sampler )
	{
		m_combinedTextureSamplers.emplace_back( layoutBinding
			, view
			, sampler );
		return m_combinedTextureSamplers.back();
	}

	renderer::SamplerBinding const & DescriptorSet::createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, renderer::Sampler const & sampler )
	{
		m_samplers.emplace_back( layoutBinding
			, sampler );
		return m_samplers.back();
	}

	renderer::SampledTextureBinding const & DescriptorSet::createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, renderer::TextureView const & view
		, renderer::ImageLayout layout )
	{
		m_sampledTextures.emplace_back( layoutBinding
			, view
			, layout );
		return m_sampledTextures.back();
	}

	renderer::StorageTextureBinding const & DescriptorSet::createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, renderer::TextureView const & view )
	{
		m_storageTextures.emplace_back( layoutBinding
			, view );
		return m_storageTextures.back();
	}

	renderer::UniformBufferBinding const & DescriptorSet::createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, renderer::UniformBufferBase const & uniformBuffer
		, uint32_t offset
		, uint32_t range )
	{
		m_uniformBuffers.emplace_back( layoutBinding
			, uniformBuffer
			, offset
			, range );
		return m_uniformBuffers.back();
	}

	renderer::StorageBufferBinding const & DescriptorSet::createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, renderer::BufferBase const & storageBuffer
		, uint32_t offset
		, uint32_t range )
	{
		m_storageBuffers.emplace_back( layoutBinding
			, storageBuffer
			, offset
			, range );
		return m_storageBuffers.back();
	}

	renderer::TexelBufferBinding const & DescriptorSet::createBinding( renderer::DescriptorSetLayoutBinding const & layoutBinding
		, renderer::BufferBase const & buffer
		, renderer::BufferView const & view )
	{
		m_texelBuffers.emplace_back( layoutBinding
			, buffer
			, view );
		return m_texelBuffers.back();
	}

	void DescriptorSet::update()const
	{
	}
}
