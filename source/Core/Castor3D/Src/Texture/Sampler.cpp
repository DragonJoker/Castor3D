#include "Sampler.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderTarget.hpp"

using namespace Castor;

namespace Castor3D
{
	Sampler::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< Sampler >{ p_tabs }
	{
	}

	bool Sampler::TextWriter::operator()( Sampler const & p_sampler, TextFile & p_file )
	{
		bool l_return = true;

		Logger::LogInfo( m_tabs + cuT( "Writing Sampler " ) + p_sampler.GetName() );
		if ( p_sampler.GetName() != cuT( "LightsSampler" ) && p_sampler.GetName() != RenderTarget::DefaultSamplerName )
		{
			static std::map< InterpolationMode, String > MapInterpolationModes
			{
				{ InterpolationMode::Nearest, cuT( "nearest" ) },
				{ InterpolationMode::Linear, cuT( "linear" ) },
			};
			static std::map< WrapMode, String > MapWrappingModes
			{
				{ WrapMode::Repeat, cuT( "repeat" ) },
				{ WrapMode::MirroredRepeat, cuT( "mirrored_repeat" ) },
				{ WrapMode::ClampToBorder, cuT( "clamp_to_border" ) },
				{ WrapMode::ClampToEdge, cuT( "clamp_to_edge" ) },
			};

			l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "sampler \"" ) + p_sampler.GetName() + cuT( "\"\n" ) ) > 0
				&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
			Castor::TextWriter< Sampler >::CheckError( l_return, "Sampler name" );

			if ( l_return && p_sampler.GetInterpolationMode( InterpolationFilter::Min ) != InterpolationMode::Undefined )
			{
				l_return = p_file.WriteText( m_tabs + cuT( "\tmin_filter " ) + MapInterpolationModes[p_sampler.GetInterpolationMode( InterpolationFilter::Min )] + cuT( "\n" ) ) > 0;
				Castor::TextWriter< Sampler >::CheckError( l_return, "Sampler min filter" );
			}

			if ( l_return && p_sampler.GetInterpolationMode( InterpolationFilter::Mag ) != InterpolationMode::Undefined )
			{
				l_return = p_file.WriteText( m_tabs + cuT( "\tmag_filter " ) + MapInterpolationModes[p_sampler.GetInterpolationMode( InterpolationFilter::Mag )] + cuT( "\n" ) ) > 0;
				Castor::TextWriter< Sampler >::CheckError( l_return, "Sampler mag filter" );
			}

			if ( l_return && p_sampler.GetInterpolationMode( InterpolationFilter::Mip ) != InterpolationMode::Undefined )
			{
				l_return = p_file.WriteText( m_tabs + cuT( "\tmip_filter " ) + MapInterpolationModes[p_sampler.GetInterpolationMode( InterpolationFilter::Mip )] + cuT( "\n" ) ) > 0;
				Castor::TextWriter< Sampler >::CheckError( l_return, "Sampler mip filter" );
			}

			if ( l_return && p_sampler.GetWrappingMode( TextureUVW::U ) != WrapMode::Count )
			{
				l_return = p_file.WriteText( m_tabs + cuT( "\tu_wrap_mode " ) + MapWrappingModes[p_sampler.GetWrappingMode( TextureUVW::U )] + cuT( "\n" ) ) > 0;
				Castor::TextWriter< Sampler >::CheckError( l_return, "Sampler U wrap mode" );
			}

			if ( l_return && p_sampler.GetWrappingMode( TextureUVW::V ) != WrapMode::Count )
			{
				l_return = p_file.WriteText( m_tabs + cuT( "\tv_wrap_mode " ) + MapWrappingModes[p_sampler.GetWrappingMode( TextureUVW::V )] + cuT( "\n" ) ) > 0;
				Castor::TextWriter< Sampler >::CheckError( l_return, "Sampler V wrap mode" );
			}

			if ( l_return && p_sampler.GetWrappingMode( TextureUVW::W ) != WrapMode::Count )
			{
				l_return = p_file.WriteText( m_tabs + cuT( "\tw_wrap_mode " ) + MapWrappingModes[p_sampler.GetWrappingMode( TextureUVW::W )] + cuT( "\n" ) ) > 0;
				Castor::TextWriter< Sampler >::CheckError( l_return, "Sampler W wrap mode" );
			}

			if ( l_return )
			{
				l_return = p_file.Print( 256, cuT( "%s\tmin_lod %.2f\n" ), m_tabs.c_str(), p_sampler.GetMinLod() ) > 0;
				Castor::TextWriter< Sampler >::CheckError( l_return, "Sampler min lod" );
			}

			if ( l_return )
			{
				l_return = p_file.Print( 256, cuT( "%s\tmax_lod %.2f\n" ), m_tabs.c_str(), p_sampler.GetMaxLod() ) > 0;
				Castor::TextWriter< Sampler >::CheckError( l_return, "Sampler max lod" );
			}

			if ( l_return )
			{
				l_return = p_file.Print( 256, cuT( "%s\tlod_bias %.2f\n" ), m_tabs.c_str(), p_sampler.GetLodBias() ) > 0;
				Castor::TextWriter< Sampler >::CheckError( l_return, "Sampler lod bias" );
			}

			if ( l_return )
			{
				l_return = p_file.Print( 256, cuT( "%s\tborder_colour " ), m_tabs.c_str() ) > 0
					&& Colour::TextWriter( String() )( p_sampler.GetBorderColour(), p_file )
					&& p_file.WriteText( cuT( "\n" ) ) > 0;
				Castor::TextWriter< Sampler >::CheckError( l_return, "Sampler border colour" );
			}

			if ( l_return )
			{
				l_return = p_file.Print( 256, cuT( "%s\tmax_anisotropy %.2f\n" ), m_tabs.c_str(), p_sampler.GetMaxAnisotropy() ) > 0;
				Castor::TextWriter< Sampler >::CheckError( l_return, "Sampler max anisotropy" );
			}

			if ( l_return )
			{
				l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
			}
		}

		return l_return;
	}

	//*********************************************************************************************

	Sampler::Sampler( Engine & p_engine, String const & p_name )
		: OwnedBy< Engine >( p_engine )
		, m_rMinLod( -1000 )
		, m_rMaxLod( 1000 )
		, m_clrBorderColour( Colour::from_components( 0, 0, 0, 0 ) )
		, m_rMaxAnisotropy( 1.0 )
		, m_name( p_name )
		, m_rLodBias( 0.0 )
	{
		m_eWrapModes[uint32_t( TextureUVW::U )] = WrapMode::Repeat;
		m_eWrapModes[uint32_t( TextureUVW::V )] = WrapMode::Repeat;
		m_eWrapModes[uint32_t( TextureUVW::W )] = WrapMode::Repeat;
		m_eInterpolationModes[uint32_t( InterpolationFilter::Min )] = InterpolationMode::Nearest;
		m_eInterpolationModes[uint32_t( InterpolationFilter::Mag )] = InterpolationMode::Nearest;
		m_eInterpolationModes[uint32_t( InterpolationFilter::Mip )] = InterpolationMode::Undefined;
	}

	Sampler::~Sampler()
	{
	}
}
