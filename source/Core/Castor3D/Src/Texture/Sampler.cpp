#include "Sampler.hpp"

#include "Engine.hpp"
#include "Render/RenderTarget.hpp"

using namespace castor;

namespace castor3d
{
	Sampler::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< Sampler >{ p_tabs }
	{
	}

	bool Sampler::TextWriter::operator()( Sampler const & p_sampler, TextFile & p_file )
	{
		bool result = true;

		Logger::logInfo( m_tabs + cuT( "Writing Sampler " ) + p_sampler.getName() );

		if ( p_sampler.getName() != cuT( "LightsSampler" ) && p_sampler.getName() != RenderTarget::DefaultSamplerName )
		{
			static std::map< InterpolationMode, String > MapInterpolationModes
			{
				{ InterpolationMode::eNearest, cuT( "nearest" ) },
				{ InterpolationMode::eLinear, cuT( "linear" ) },
			};
			static std::map< WrapMode, String > MapWrappingModes
			{
				{ WrapMode::eRepeat, cuT( "repeat" ) },
				{ WrapMode::eMirroredRepeat, cuT( "mirrored_repeat" ) },
				{ WrapMode::eClampToBorder, cuT( "clamp_to_border" ) },
				{ WrapMode::eClampToEdge, cuT( "clamp_to_edge" ) },
			};
			static std::map< ComparisonMode, String > ComparisonModes
			{
				{ ComparisonMode::eNone, cuT( "none" ) },
				{ ComparisonMode::eRefToTexture, cuT( "ref_to_texture" ) },
			};
			static std::map< ComparisonFunc, String > ComparisonFunctions
			{
				{ ComparisonFunc::eAlways, cuT( "always" ) },
				{ ComparisonFunc::eLess, cuT( "less" ) },
				{ ComparisonFunc::eLEqual, cuT( "less_or_equal" ) },
				{ ComparisonFunc::eEqual, cuT( "equal" ) },
				{ ComparisonFunc::eNEqual, cuT( "not_equal" ) },
				{ ComparisonFunc::eGEqual, cuT( "greater_or_equal" ) },
				{ ComparisonFunc::eGreater, cuT( "greater" ) },
				{ ComparisonFunc::eNever, cuT( "never" ) },
			};

			result = p_file.writeText( cuT( "\n" ) + m_tabs + cuT( "sampler \"" ) + p_sampler.getName() + cuT( "\"\n" ) ) > 0
					   && p_file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
			castor::TextWriter< Sampler >::checkError( result, "Sampler name" );

			if ( result && p_sampler.getInterpolationMode( InterpolationFilter::eMin ) != InterpolationMode::eUndefined )
			{
				result = p_file.writeText( m_tabs + cuT( "\tmin_filter " ) + MapInterpolationModes[p_sampler.getInterpolationMode( InterpolationFilter::eMin )] + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler min filter" );
			}

			if ( result && p_sampler.getInterpolationMode( InterpolationFilter::eMag ) != InterpolationMode::eUndefined )
			{
				result = p_file.writeText( m_tabs + cuT( "\tmag_filter " ) + MapInterpolationModes[p_sampler.getInterpolationMode( InterpolationFilter::eMag )] + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler mag filter" );
			}

			if ( result && p_sampler.getInterpolationMode( InterpolationFilter::eMip ) != InterpolationMode::eUndefined )
			{
				result = p_file.writeText( m_tabs + cuT( "\tmip_filter " ) + MapInterpolationModes[p_sampler.getInterpolationMode( InterpolationFilter::eMip )] + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler mip filter" );
			}

			if ( result && p_sampler.getWrappingMode( TextureUVW::eU ) != WrapMode::eCount )
			{
				result = p_file.writeText( m_tabs + cuT( "\tu_wrap_mode " ) + MapWrappingModes[p_sampler.getWrappingMode( TextureUVW::eU )] + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler U wrap mode" );
			}

			if ( result && p_sampler.getWrappingMode( TextureUVW::eV ) != WrapMode::eCount )
			{
				result = p_file.writeText( m_tabs + cuT( "\tv_wrap_mode " ) + MapWrappingModes[p_sampler.getWrappingMode( TextureUVW::eV )] + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler V wrap mode" );
			}

			if ( result && p_sampler.getWrappingMode( TextureUVW::eW ) != WrapMode::eCount )
			{
				result = p_file.writeText( m_tabs + cuT( "\tw_wrap_mode " ) + MapWrappingModes[p_sampler.getWrappingMode( TextureUVW::eW )] + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler W wrap mode" );
			}

			if ( result )
			{
				result = p_file.print( 256, cuT( "%s\tmin_lod %.2f\n" ), m_tabs.c_str(), p_sampler.getMinLod() ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler min lod" );
			}

			if ( result )
			{
				result = p_file.print( 256, cuT( "%s\tmax_lod %.2f\n" ), m_tabs.c_str(), p_sampler.getMaxLod() ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler max lod" );
			}

			if ( result )
			{
				result = p_file.print( 256, cuT( "%s\tlod_bias %.2f\n" ), m_tabs.c_str(), p_sampler.getLodBias() ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler lod bias" );
			}

			if ( result && p_sampler.getComparisonMode() != ComparisonMode::eNone )
			{
				result = p_file.writeText( m_tabs + cuT( "\tcomparison_mode " ) + ComparisonModes[p_sampler.getComparisonMode()] + cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler comparison mode" );

				if ( result )
				{
					result = p_file.writeText( m_tabs + cuT( "\tcomparison_func " ) + ComparisonFunctions[p_sampler.getComparisonFunc()] + cuT( "\n" ) ) > 0;
					castor::TextWriter< Sampler >::checkError( result, "Sampler comparison function" );
				}
			}

			if ( result )
			{
				result = p_file.print( 256, cuT( "%s\tborder_colour " ), m_tabs.c_str() ) > 0
						   && RgbaColour::TextWriter( String() )( p_sampler.getBorderColour(), p_file )
						   && p_file.writeText( cuT( "\n" ) ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler border colour" );
			}

			if ( result )
			{
				result = p_file.print( 256, cuT( "%s\tmax_anisotropy %.2f\n" ), m_tabs.c_str(), p_sampler.getMaxAnisotropy() ) > 0;
				castor::TextWriter< Sampler >::checkError( result, "Sampler max anisotropy" );
			}

			if ( result )
			{
				result = p_file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
			}
		}

		return result;
	}

	//*********************************************************************************************

	Sampler::Sampler( Engine & engine, String const & p_name )
		: OwnedBy< Engine >{ engine }
		, Named{ p_name }
	{
	}

	Sampler::~Sampler()
	{
	}
}
