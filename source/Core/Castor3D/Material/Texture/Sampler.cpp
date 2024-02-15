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
	namespace sampler
	{
		static castor::String getName( VkCompareOp value )
		{
			switch ( value )
			{
			case VK_COMPARE_OP_NEVER:
				return cuT( "Nev" );
			case VK_COMPARE_OP_LESS:
				return cuT( "Lss" );
			case VK_COMPARE_OP_EQUAL:
				return cuT( "Eq" );
			case VK_COMPARE_OP_LESS_OR_EQUAL:
				return cuT( "LEq" );
			case VK_COMPARE_OP_GREATER:
				return cuT( "Gtr" );
			case VK_COMPARE_OP_NOT_EQUAL:
				return cuT( "NEq" );
			case VK_COMPARE_OP_GREATER_OR_EQUAL:
				return cuT( "GEq" );
			case VK_COMPARE_OP_ALWAYS:
				return cuT( "Alw" );
			default:
				assert( false && "Unsupported VkCompareOp." );
				return cuT( "Unsupported VkCompareOp" );
			}
		}

		static castor::String getName( VkFilter value )
		{
			switch ( value )
			{
			case VK_FILTER_NEAREST:
				return cuT( "Near" );
			case VK_FILTER_LINEAR:
				return cuT( "Lin" );
			case VK_FILTER_CUBIC_IMG:
				return cuT( "Cub" );
			default:
				assert( false && "Unsupported VkFilter." );
				return cuT( "Unsupported VkFilter" );
			}
		}

		static castor::String getName( VkSamplerMipmapMode value )
		{
			switch ( value )
			{
			case VK_SAMPLER_MIPMAP_MODE_NEAREST:
				return cuT( "Near" );
			case VK_SAMPLER_MIPMAP_MODE_LINEAR:
				return cuT( "Lin" );
			default:
				assert( false && "Unsupported VkSamplerMipmapMode." );
				return cuT( "Unsupported VkSamplerMipmapMode" );
			}
		}

		static castor::String getName( VkSamplerAddressMode value )
		{
			switch ( value )
			{
			case VK_SAMPLER_ADDRESS_MODE_REPEAT:
				return cuT( "Rep" );
			case VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT:
				return cuT( "MRep" );
			case VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:
				return cuT( "CtE" );
			case VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER:
				return cuT( "CtB" );
			case VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE:
				return cuT( "MCtE" );
			default:
				assert( false && "Unsupported VkSamplerAddressMode." );
				return cuT( "Unsupported VkSamplerAddressMode" );
			}
		}

		static castor::String getName( VkBorderColor value )
		{
			switch ( value )
			{
			case VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK:
				return cuT( "Ftb" );
			case VK_BORDER_COLOR_INT_TRANSPARENT_BLACK:
				return cuT( "Itb" );
			case VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK:
				return cuT( "Fob" );
			case VK_BORDER_COLOR_INT_OPAQUE_BLACK:
				return cuT( "Iob" );
			case VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE:
				return cuT( "Fow" );
			case VK_BORDER_COLOR_INT_OPAQUE_WHITE:
				return cuT( "Iow" );
			case VK_BORDER_COLOR_FLOAT_CUSTOM_EXT:
				return cuT( "Fc" );
			case VK_BORDER_COLOR_INT_CUSTOM_EXT:
				return cuT( "Ic" );
			default:
				assert( false && "Unsupported VkBorderColor." );
				return cuT( "Unsupported VkBorderColor" );
			}
		}
	}

	SamplerObs createSampler( Engine & engine
		, castor::String const & baseName
		, VkFilter filter
		, VkImageSubresourceRange const * range )
	{
		castor::String const name = baseName
			+ cuT( "_" ) + castor::makeString( ashes::getName( filter ) )
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
				, castor::move( createInfo ) );
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
		, VkSamplerAddressMode W
		, VkBorderColor borderColor )
	{
		return sampler::getName( compareOp )
			+ sampler::getName( minFilter )
			+ sampler::getName( magFilter )
			+ sampler::getName( mipFilter )
			+ cuT( "U" ) + sampler::getName( U )
			+ cuT( "V" ) + sampler::getName( V )
			+ cuT( "W" ) + sampler::getName( W )
			+ sampler::getName( borderColor );
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
		, ashes::SamplerCreateInfo const & createInfo )
		: castor::Named{ name }
		, castor::OwnedBy< Engine >{ engine }
		, m_info{ static_cast< VkSamplerCreateInfo const & >( createInfo ) }
	{
		CU_Require( m_info.sType == VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO );
	}

	bool Sampler::initialise( RenderDevice const & device )
	{
		if ( !m_initialised
			&& !m_sampler
			&& !m_initialising.exchange( true ) )
		{
			CU_Require( m_info.sType == VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO );
			m_info.maxAnisotropy = std::min( m_info.maxAnisotropy, device.properties.limits.maxSamplerAnisotropy );
			m_sampler = device->createSampler( castor::toUtf8( getName() )
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
