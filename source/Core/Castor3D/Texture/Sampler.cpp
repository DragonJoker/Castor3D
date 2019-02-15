#include "Castor3D/Texture/Sampler.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"

#include <Ashes/Core/Device.hpp>
#include <Ashes/Core/PhysicalDevice.hpp>

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
				result = file.writeText( m_tabs + cuT( "\tmin_filter " ) + ashes::getName( sampler.getMinFilter() ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler min filter" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tmag_filter " ) + ashes::getName( sampler.getMagFilter() ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler mag filter" );
			}

			if ( result && sampler.getMipFilter() != ashes::MipmapMode::eNone )
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
				result = file.writeText( m_tabs + cuT( "\tmin_lod " ) + string::toString( sampler.getMinLod(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler min lod" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tmax_lod " ) + string::toString( sampler.getMaxLod(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler max lod" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tlod_bias " ) + string::toString( sampler.getLodBias(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler lod bias" );
			}

			if ( result
				&& sampler.getCompareOp() != ashes::CompareOp::eNever
				&& sampler.getCompareOp() != ashes::CompareOp::eAlways )
			{
				result = file.writeText( m_tabs + cuT( "\tcomparison_mode ref_to_texture\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler comparison mode" );

				if ( result )
				{
					result = file.writeText( m_tabs + cuT( "\tcomparison_func " ) + ashes::getName( sampler.getCompareOp() ) + cuT( "\n" ) ) > 0;
					castor::TextWriter< Sampler >::checkError( result, "Sampler comparison function" );
				}
			}

			if ( result && sampler.getBorderColour() != ashes::BorderColour::eFloatOpaqueBlack )
			{
				result = file.writeText( m_tabs + cuT( "\tborder_colour " ) + ashes::getName( sampler.getBorderColour() ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler border colour" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tmax_anisotropy " ) + string::toString( sampler.getMaxAnisotropy(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
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
			auto & device = getCurrentDevice( *this );
			m_info.maxAnisotropy = std::min( m_info.maxAnisotropy, device.getPhysicalDevice().getProperties().limits.maxSamplerAnisotropy );
			m_sampler = device.createSampler( m_info );
		}

		return true;
	}

	void Sampler::cleanup()
	{
		m_sampler.reset();
	}
}
