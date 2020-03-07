#include "Castor3D/Material/Texture/Sampler.hpp"

#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderTarget.hpp"

namespace castor3d
{
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

	Sampler::~Sampler()
	{
	}

	bool Sampler::initialise()
	{
		if ( !m_sampler )
		{
			auto & device = getCurrentRenderDevice( *this );
			m_info->maxAnisotropy = std::min( m_info->maxAnisotropy, device.properties.limits.maxSamplerAnisotropy );
			m_sampler = device->createSampler( m_info );
			setDebugObjectName( device, *m_sampler, getName() + "Sampler" );
		}

		return true;
	}

	void Sampler::cleanup()
	{
		m_sampler.reset();
	}
}
