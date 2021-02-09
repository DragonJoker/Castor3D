#include "Castor3D/Material/Texture/Sampler.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
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

		return sampler;
	}

	Sampler::TextWriter::TextWriter( castor::String const & tabs )
		: castor::TextWriter< Sampler >{ tabs }
	{
	}

	bool Sampler::TextWriter::operator()( Sampler const & sampler
		, castor::TextFile & file )
	{
		bool result = true;

		log::info << m_tabs << cuT( "Writing Sampler " ) << sampler.getName() << std::endl;

		if ( sampler.getName() != cuT( "LightsSampler" ) && sampler.getName() != RenderTarget::DefaultSamplerName )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "sampler \"" ) + sampler.getName() + cuT( "\"\n" ) ) > 0
					   && file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
			castor::TextWriter< Sampler >::checkError( result, "Sampler name" );

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tmin_filter " ) + ashes::getName( sampler.getMinFilter() ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler min filter" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tmag_filter " ) + ashes::getName( sampler.getMagFilter() ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler mag filter" );
			}

			if ( result && sampler.getMipFilter() != VK_SAMPLER_MIPMAP_MODE_NEAREST )
			{
				result = file.writeText( m_tabs + cuT( "\tmip_filter " ) + ashes::getName( sampler.getMipFilter() ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler mip filter" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tu_wrap_mode " ) + ashes::getName( sampler.getWrapS() ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler U wrap mode" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tv_wrap_mode " ) + ashes::getName( sampler.getWrapT() ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler V wrap mode" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tw_wrap_mode " ) + ashes::getName( sampler.getWrapR() ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler W wrap mode" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tmin_lod " ) + castor::string::toString( sampler.getMinLod(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler min lod" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tmax_lod " ) + castor::string::toString( sampler.getMaxLod(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler max lod" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tlod_bias " ) + castor::string::toString( sampler.getLodBias(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler lod bias" );
			}

			if ( result
				&& sampler.getCompareOp() != VK_COMPARE_OP_NEVER
				&& sampler.getCompareOp() != VK_COMPARE_OP_ALWAYS )
			{
				result = file.writeText( m_tabs + cuT( "\tcomparison_mode ref_to_texture\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler comparison mode" );

				if ( result )
				{
					result = file.writeText( m_tabs + cuT( "\tcomparison_func " ) + ashes::getName( sampler.getCompareOp() ) + cuT( "\n" ) ) > 0;
					castor::TextWriter< Sampler >::checkError( result, "Sampler comparison function" );
				}
			}

			if ( result && sampler.getBorderColour() != VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK )
			{
				result = file.writeText( m_tabs + cuT( "\tborder_colour " ) + ashes::getName( sampler.getBorderColour() ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler border colour" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tmax_anisotropy " ) + castor::string::toString( sampler.getMaxAnisotropy(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler max anisotropy" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
			}
		}

		return result;
	}

	//*********************************************************************************************

	Sampler::Sampler( Engine & engine
		, castor::String const & name )
		: castor::OwnedBy< Engine >{ engine }
		, castor::Named{ name }
	{
	}
	
	Sampler::Sampler( Engine & engine
		, castor::String const & name
		, ashes::SamplerCreateInfo createInfo )
		: castor::OwnedBy< Engine >{ engine }
		, castor::Named{ name }
		, m_info{ std::move( createInfo ) }
	{
	}

	Sampler::~Sampler()
	{
	}

	bool Sampler::initialise( RenderDevice const & device )
	{
		if ( !m_sampler )
		{
			m_info->maxAnisotropy = std::min( m_info->maxAnisotropy, device.properties.limits.maxSamplerAnisotropy );
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
