#include "Sampler.hpp"

#include "Engine.hpp"
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
		bool result = true;

		Logger::LogInfo( m_tabs + cuT( "Writing Sampler " ) + p_sampler.GetName() );

		if ( p_sampler.GetName() != cuT( "LightsSampler" ) && p_sampler.GetName() != RenderTarget::DefaultSamplerName )
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

			result = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "sampler \"" ) + p_sampler.GetName() + cuT( "\"\n" ) ) > 0
					   && p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
			Castor::TextWriter< Sampler >::CheckError( result, "Sampler name" );

			if ( result && p_sampler.GetInterpolationMode( InterpolationFilter::eMin ) != InterpolationMode::eUndefined )
			{
				result = p_file.WriteText( m_tabs + cuT( "\tmin_filter " ) + MapInterpolationModes[p_sampler.GetInterpolationMode( InterpolationFilter::eMin )] + cuT( "\n" ) ) > 0;
				Castor::TextWriter< Sampler >::CheckError( result, "Sampler min filter" );
			}

			if ( result && p_sampler.GetInterpolationMode( InterpolationFilter::eMag ) != InterpolationMode::eUndefined )
			{
				result = p_file.WriteText( m_tabs + cuT( "\tmag_filter " ) + MapInterpolationModes[p_sampler.GetInterpolationMode( InterpolationFilter::eMag )] + cuT( "\n" ) ) > 0;
				Castor::TextWriter< Sampler >::CheckError( result, "Sampler mag filter" );
			}

			if ( result && p_sampler.GetInterpolationMode( InterpolationFilter::eMip ) != InterpolationMode::eUndefined )
			{
				result = p_file.WriteText( m_tabs + cuT( "\tmip_filter " ) + MapInterpolationModes[p_sampler.GetInterpolationMode( InterpolationFilter::eMip )] + cuT( "\n" ) ) > 0;
				Castor::TextWriter< Sampler >::CheckError( result, "Sampler mip filter" );
			}

			if ( result && p_sampler.GetWrappingMode( TextureUVW::eU ) != WrapMode::eCount )
			{
				result = p_file.WriteText( m_tabs + cuT( "\tu_wrap_mode " ) + MapWrappingModes[p_sampler.GetWrappingMode( TextureUVW::eU )] + cuT( "\n" ) ) > 0;
				Castor::TextWriter< Sampler >::CheckError( result, "Sampler U wrap mode" );
			}

			if ( result && p_sampler.GetWrappingMode( TextureUVW::eV ) != WrapMode::eCount )
			{
				result = p_file.WriteText( m_tabs + cuT( "\tv_wrap_mode " ) + MapWrappingModes[p_sampler.GetWrappingMode( TextureUVW::eV )] + cuT( "\n" ) ) > 0;
				Castor::TextWriter< Sampler >::CheckError( result, "Sampler V wrap mode" );
			}

			if ( result && p_sampler.GetWrappingMode( TextureUVW::eW ) != WrapMode::eCount )
			{
				result = p_file.WriteText( m_tabs + cuT( "\tw_wrap_mode " ) + MapWrappingModes[p_sampler.GetWrappingMode( TextureUVW::eW )] + cuT( "\n" ) ) > 0;
				Castor::TextWriter< Sampler >::CheckError( result, "Sampler W wrap mode" );
			}

			if ( result )
			{
				result = p_file.Print( 256, cuT( "%s\tmin_lod %.2f\n" ), m_tabs.c_str(), p_sampler.GetMinLod() ) > 0;
				Castor::TextWriter< Sampler >::CheckError( result, "Sampler min lod" );
			}

			if ( result )
			{
				result = p_file.Print( 256, cuT( "%s\tmax_lod %.2f\n" ), m_tabs.c_str(), p_sampler.GetMaxLod() ) > 0;
				Castor::TextWriter< Sampler >::CheckError( result, "Sampler max lod" );
			}

			if ( result )
			{
				result = p_file.Print( 256, cuT( "%s\tlod_bias %.2f\n" ), m_tabs.c_str(), p_sampler.GetLodBias() ) > 0;
				Castor::TextWriter< Sampler >::CheckError( result, "Sampler lod bias" );
			}

			if ( result && p_sampler.GetComparisonMode() != ComparisonMode::eNone )
			{
				result = p_file.WriteText( m_tabs + cuT( "\tcomparison_mode " ) + ComparisonModes[p_sampler.GetComparisonMode()] + cuT( "\n" ) ) > 0;
				Castor::TextWriter< Sampler >::CheckError( result, "Sampler comparison mode" );

				if ( result )
				{
					result = p_file.WriteText( m_tabs + cuT( "\tcomparison_func " ) + ComparisonFunctions[p_sampler.GetComparisonFunc()] + cuT( "\n" ) ) > 0;
					Castor::TextWriter< Sampler >::CheckError( result, "Sampler comparison function" );
				}
			}

			if ( result )
			{
				result = p_file.Print( 256, cuT( "%s\tborder_colour " ), m_tabs.c_str() ) > 0
						   && Colour::TextWriter( String() )( p_sampler.GetBorderColour(), p_file )
						   && p_file.WriteText( cuT( "\n" ) ) > 0;
				Castor::TextWriter< Sampler >::CheckError( result, "Sampler border colour" );
			}

			if ( result )
			{
				result = p_file.Print( 256, cuT( "%s\tmax_anisotropy %.2f\n" ), m_tabs.c_str(), p_sampler.GetMaxAnisotropy() ) > 0;
				Castor::TextWriter< Sampler >::CheckError( result, "Sampler max anisotropy" );
			}

			if ( result )
			{
				result = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
			}
		}

		return result;
	}

	//*********************************************************************************************

	Sampler::Sampler( Engine & p_engine, String const & p_name )
		: OwnedBy< Engine >{ p_engine }
		, Named{ p_name }
	{
	}

	Sampler::~Sampler()
	{
	}
}
