#include "Descriptor/GlDescriptorSet.hpp"

#include "Buffer/GlBuffer.hpp"
#include "Descriptor/GlDescriptorPool.hpp"
#include "Image/GlSampler.hpp"
#include "Image/GlTexture.hpp"
#include "Buffer/GlUniformBuffer.hpp"

#include <Descriptor/DescriptorSetLayoutBinding.hpp>

#include <algorithm>

namespace gl_renderer
{
	DescriptorSet::DescriptorSet( renderer::DescriptorPool const & pool
		, renderer::DescriptorSetLayout const & layout
		, uint32_t bindingPoint )
		: renderer::DescriptorSet{ pool, bindingPoint }
	{
	}

	void DescriptorSet::update()const
	{
		for ( auto & write : m_writes )
		{
			switch ( write.descriptorType )
			{
			case renderer::DescriptorType::eSampler:
				m_samplers.push_back( write );
				break;
			case renderer::DescriptorType::eCombinedImageSampler:
				m_combinedTextureSamplers.push_back( write );
				break;
			case renderer::DescriptorType::eSampledImage:
				m_sampledTextures.push_back( write );
				break;
			case renderer::DescriptorType::eStorageImage:
				m_storageTextures.push_back( write );
				break;
			case renderer::DescriptorType::eUniformTexelBuffer:
				m_texelBuffers.push_back( write );
				break;
			case renderer::DescriptorType::eStorageTexelBuffer:
				m_texelBuffers.push_back( write );
				break;
			case renderer::DescriptorType::eUniformBuffer:
				m_uniformBuffers.push_back( write );
				break;
			case renderer::DescriptorType::eStorageBuffer:
				m_storageBuffers.push_back( write );
				break;
			case renderer::DescriptorType::eUniformBufferDynamic:
				m_dynamicUniformBuffers.push_back( write );
				m_dynamicBuffers.push_back( write );
				break;
			case renderer::DescriptorType::eStorageBufferDynamic:
				m_dynamicStorageBuffers.push_back( write );
				m_dynamicBuffers.push_back( write );
				break;
			case renderer::DescriptorType::eInputAttachment:
				break;
			}
		}

		std::sort( m_dynamicBuffers.begin()
			, m_dynamicBuffers.end()
			, []( renderer::WriteDescriptorSet const & lhs
				, renderer::WriteDescriptorSet const & rhs )
		{
			return lhs.dstBinding < rhs.dstBinding;
		} );
	}
}
