#include "Castor3D/Material/Texture/Sampler.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/Cache.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"

namespace castor3d
{
	SamplerSPtr createSampler( Engine & engine
		, castor::String const & baseName
		, VkFilter filter
		, VkImageSubresourceRange const * range )
	{
		castor::String const name = baseName
			+ cuT( "_" ) + ashes::getName( filter )
			+ ( range
				? cuT( "_" ) + castor::string::toString( range->baseMipLevel ) + cuT( "_" ) + castor::string::toString( range->levelCount )
				: castor::String{} );
		auto & cache = engine.getSamplerCache();
		SamplerSPtr sampler;

		if ( cache.has( name ) )
		{
			sampler = cache.find( name );
		}
		else
		{
			ashes::SamplerCreateInfo createInfo
			{
				0u,
				filter,
				filter,
				VK_SAMPLER_MIPMAP_MODE_NEAREST,
				VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
				VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
				VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
				0.0f, // mipLodBias
				VK_FALSE, // anisotropyEnable
				0.0f, // maxAnisotropy
				VK_FALSE, // compareEnable
				VK_COMPARE_OP_ALWAYS, // compareOp
				( range
					? float( range->baseMipLevel )
					: -1000.0f ), // minLod
				( range
					? float( range->baseMipLevel + range->levelCount )
					: 1000.0f ), // maxLod
			};
			sampler = std::make_shared< Sampler >( engine
				, name
				, std::move( createInfo ) );
			cache.add( name, sampler );
		}

		sampler->initialise( engine.getRenderSystem()->getRenderDevice() );
		return sampler;
	}

	//*********************************************************************************************

	Sampler::Sampler( Engine & engine
		, castor::String const & name )
		: castor::OwnedBy< Engine >{ engine }
		, castor::Named{ name }
	{
		CU_Require( m_info.sType == VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO );
	}
	
	Sampler::Sampler( Engine & engine
		, castor::String const & name
		, ashes::SamplerCreateInfo createInfo )
		: castor::OwnedBy< Engine >{ engine }
		, castor::Named{ name }
		, m_info{ static_cast< VkSamplerCreateInfo const & >( createInfo ) }
	{
		CU_Require( m_info.sType == VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO );
	}

	Sampler::~Sampler()
	{
	}

	bool Sampler::initialise( RenderDevice const & device )
	{
		if ( !m_sampler )
		{
			CU_Require( m_info.sType == VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO );
			m_info.maxAnisotropy = std::min( m_info.maxAnisotropy, device.properties.limits.maxSamplerAnisotropy );
			m_sampler = device->createSampler( getName() + "Sampler"
				, m_info );
		}

		return true;
	}

	void Sampler::cleanup()
	{
		m_sampler.reset();
	}
}
