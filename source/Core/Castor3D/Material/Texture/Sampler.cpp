#include "Castor3D/Material/Texture/Sampler.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

CU_ImplementSmartPtr( castor3d, Sampler )

namespace castor3d
{
	SamplerObs createSampler( Engine & engine
		, castor::String const & baseName
		, VkFilter filter
		, VkImageSubresourceRange const * range )
	{
		castor::String const name = baseName
			+ cuT( "_" ) + ashes::getName( filter )
			+ ( range
				? cuT( "_" ) + castor::string::toString( range->baseMipLevel ) + cuT( "_" ) + castor::string::toString( range->levelCount )
				: castor::String{} );
		SamplerObs sampler{};

		if ( engine.hasSampler( name ) )
		{
			sampler = engine.findSampler( name );
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
			auto resource = engine.createSampler( name
				, engine
				, std::move( createInfo ) );
			sampler = engine.addNewSampler( name
				, resource
				, false );
		}

		sampler->initialise( engine.getRenderSystem()->getRenderDevice() );
		return sampler;
	}

	castor::String getSamplerName( VkCompareOp compareOp
		, VkFilter minFilter
		, VkFilter magFilter
		, VkSamplerMipmapMode mipFilter
		, VkSamplerAddressMode U
		, VkSamplerAddressMode V
		, VkSamplerAddressMode W )
	{
		return ashes::getName( compareOp )
			+ ashes::getName( minFilter )
			+ ashes::getName( magFilter )
			+ ashes::getName( mipFilter )
			+ ashes::getName( U )
			+ ashes::getName( V )
			+ ashes::getName( W );
	}

	//*********************************************************************************************

	Sampler::Sampler( castor::String const & name
		, Engine & engine )
		: castor::Named{ name }
		, castor::OwnedBy< Engine >{ engine }
	{
		CU_Require( m_info.sType == VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO );
	}
	
	Sampler::Sampler( castor::String const & name
		, Engine & engine
		, ashes::SamplerCreateInfo createInfo )
		: castor::Named{ name }
		, castor::OwnedBy< Engine >{ engine }
		, m_info{ static_cast< VkSamplerCreateInfo const & >( createInfo ) }
	{
		CU_Require( m_info.sType == VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO );
	}

	Sampler::~Sampler()
	{
	}

	bool Sampler::initialise( RenderDevice const & device )
	{
		if ( !m_initialised
			&& !m_sampler
			&& !m_initialising.exchange( true ) )
		{
			CU_Require( m_info.sType == VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO );
			m_info.maxAnisotropy = std::min( m_info.maxAnisotropy, device.properties.limits.maxSamplerAnisotropy );
			m_sampler = device->createSampler( getName()
				, m_info );
			m_initialised = true;
			m_initialising = false;
		}

		return true;
	}

	void Sampler::cleanup()
	{
		m_initialised = false;
		m_sampler.reset();
	}
}
