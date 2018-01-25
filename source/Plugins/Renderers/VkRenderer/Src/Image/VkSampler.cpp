#include "Image/VkSampler.hpp"

#include "Core/VkDevice.hpp"

namespace vk_renderer
{
	Sampler::Sampler( Device const & device
		, renderer::WrapMode wrapS
		, renderer::WrapMode wrapT
		, renderer::WrapMode wrapR
		, renderer::Filter minFilter
		, renderer::Filter magFilter
		, renderer::MipmapMode mipFilter
		, float minLod
		, float maxLod
		, float lodBias
		, renderer::BorderColour borderColour
		, float maxAnisotropy
		, renderer::CompareOp compareOp )
		: renderer::Sampler{ device
			, wrapS
			, wrapT
			, wrapR
			, minFilter
			, magFilter
			, mipFilter
			, minLod
			, maxLod
			, lodBias
			, borderColour
			, maxAnisotropy
			, compareOp }
		, m_device{ device }
	{
		VkSamplerCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,                // sType
			nullptr,                                              // pNext
			0,                                                    // flags
			convert( minFilter ),                                 // magFilter
			convert( magFilter ),                                 // minFilter
			convert( mipFilter ),                                 // mipmapMode
			convert( wrapS ),                                     // addressModeU
			convert( wrapT ),                                     // addressModeV
			convert( wrapR ),                                     // addressModeW
			lodBias,                                              // mipLodBias
			maxAnisotropy != 1.0f,                                // anisotropyEnable
			maxAnisotropy,                                        // maxAnisotropy
			compareOp != renderer::CompareOp::eAlways,            // compareEnable
			convert( compareOp ),                                 // compareOp
			minLod,                                               // minLod
			maxLod,                                               // maxLod
			convert( borderColour ),                              // borderColor
			VK_FALSE                                              // unnormalizedCoordinates
		};
		DEBUG_DUMP( createInfo );
		auto res = vk::CreateSampler( m_device, &createInfo, nullptr, &m_sampler );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Sampler creation failed: " + getLastError() };
		}
	}

	Sampler::~Sampler()
	{
		vk::DestroySampler( m_device, m_sampler, nullptr );
	}
}
