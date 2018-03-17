#include "Sampler.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderTarget.hpp"

#include <Core/Device.hpp>

using namespace castor;

namespace castor3d
{
	Sampler::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< Sampler >{ tabs }
	{
	}

	bool Sampler::TextWriter::operator()( Sampler const & sampler, TextFile & file )
	{
		bool result = true;

		Logger::logInfo( m_tabs + cuT( "Writing Sampler " ) + sampler.getName() );

		if ( sampler.getName() != cuT( "LightsSampler" ) && sampler.getName() != RenderTarget::DefaultSamplerName )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "sampler \"" ) + sampler.getName() + cuT( "\"\n" ) ) > 0
					   && file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
			castor::TextWriter< Sampler >::checkError( result, "Sampler name" );

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tmin_filter " ) + renderer::getName( sampler.getMinFilter() ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler min filter" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tmag_filter " ) + renderer::getName( sampler.getMagFilter() ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler mag filter" );
			}

			if ( result && sampler.getMipFilter() != renderer::MipmapMode::eNone )
			{
				result = file.writeText( m_tabs + cuT( "\tmip_filter " ) + renderer::getName( sampler.getMipFilter() ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler mip filter" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tu_wrap_mode " ) + renderer::getName( sampler.getWrapS() ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler U wrap mode" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tv_wrap_mode " ) + renderer::getName( sampler.getWrapT() ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler V wrap mode" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tw_wrap_mode " ) + renderer::getName( sampler.getWrapR() ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler W wrap mode" );
			}

			if ( result )
			{
				result = file.print( 256, cuT( "%s\tmin_lod %.2f\n" ), m_tabs.c_str(), sampler.getMinLod() ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler min lod" );
			}

			if ( result )
			{
				result = file.print( 256, cuT( "%s\tmax_lod %.2f\n" ), m_tabs.c_str(), sampler.getMaxLod() ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler max lod" );
			}

			if ( result )
			{
				result = file.print( 256, cuT( "%s\tlod_bias %.2f\n" ), m_tabs.c_str(), sampler.getLodBias() ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler lod bias" );
			}

			if ( result && sampler.getCompareOp() != renderer::CompareOp::eAlways )
			{
				result = file.writeText( m_tabs + cuT( "\tcomparison_mode ref_to_texture\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler comparison mode" );

				if ( result )
				{
					result = file.writeText( m_tabs + cuT( "\tcomparison_func " ) + renderer::getName( sampler.getCompareOp() ) + cuT( "\n" ) ) > 0;
					castor::TextWriter< Sampler >::checkError( result, "Sampler comparison function" );
				}
			}

			if ( result && sampler.getBorderColour() != renderer::BorderColour::eFloatOpaqueBlack )
			{
				result = file.writeText( m_tabs + cuT( "\tborder_colour " ) + renderer::getName( sampler.getBorderColour() ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler border colour" );
			}

			if ( result )
			{
				result = file.print( 256, cuT( "%s\tmax_anisotropy %.2f\n" ), m_tabs.c_str(), sampler.getMaxAnisotropy() ) > 0;
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

	Sampler::Sampler( Engine & engine, String const & name )
		: OwnedBy< Engine >{ engine }
		, Named{ name }
	{
	}

	Sampler::~Sampler()
	{
	}

	bool Sampler::initialise()
	{
		if ( !m_sampler )
		{
			auto device = getEngine()->getRenderSystem()->getCurrentDevice();
			REQUIRE( device );
			m_sampler = device->createSampler( m_info );
		}

		return true;
	}

	void Sampler::cleanup()
	{
		m_sampler.reset();
	}
}
