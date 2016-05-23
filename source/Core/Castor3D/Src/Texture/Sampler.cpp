#include "Sampler.hpp"

#include "Render/RenderTarget.hpp"

using namespace Castor;

namespace Castor3D
{
	Sampler::TextLoader::TextLoader( String const & p_tabs, File::eENCODING_MODE p_encodingMode )
		: Castor::TextLoader< Sampler >( p_tabs, p_encodingMode )
	{
	}

	bool Sampler::TextLoader::operator()( Sampler const & p_sampler, TextFile & p_file )
	{
		bool l_return = true;

		if ( p_sampler.GetName() != cuT( "LightsSampler" ) && p_sampler.GetName() != RenderTarget::DefaultSamplerName )
		{
			using StrUIntMap = std::map< uint32_t, String >;

			StrUIntMap l_mapInterpolationModes
			{
				{ eINTERPOLATION_MODE_NEAREST, cuT( "nearest" ) },
				{ eINTERPOLATION_MODE_LINEAR, cuT( "linear" ) },
			};
			StrUIntMap l_mapWrappingModes
			{
				{ eWRAP_MODE_REPEAT, cuT( "repeat" ) },
				{ eWRAP_MODE_MIRRORED_REPEAT, cuT( "mirrored_repeat" ) },
				{ eWRAP_MODE_CLAMP_TO_BORDER, cuT( "clamp_to_border" ) },
				{ eWRAP_MODE_CLAMP_TO_EDGE, cuT( "clamp_to_edge" ) },
			};

			l_return = p_file.WriteText( m_tabs + cuT( "sampler \"" ) + p_sampler.GetName() + cuT( "\"\n" ) ) > 0
				&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;

			if ( l_return && p_sampler.GetInterpolationMode( eINTERPOLATION_FILTER_MIN ) != eINTERPOLATION_MODE_UNDEFINED )
			{
				l_return = p_file.WriteText( m_tabs + cuT( "\tmin_filter " ) + l_mapInterpolationModes[p_sampler.GetInterpolationMode( eINTERPOLATION_FILTER_MIN )] + cuT( "\n" ) ) > 0;
			}

			if ( l_return && p_sampler.GetInterpolationMode( eINTERPOLATION_FILTER_MIN ) != eINTERPOLATION_FILTER_MAG )
			{
				l_return = p_file.WriteText( m_tabs + cuT( "\tmag_filter " ) + l_mapInterpolationModes[p_sampler.GetInterpolationMode( eINTERPOLATION_FILTER_MAG )] + cuT( "\n" ) ) > 0;
			}

			if ( l_return && p_sampler.GetInterpolationMode( eINTERPOLATION_FILTER_MIN ) != eINTERPOLATION_FILTER_MIP )
			{
				l_return = p_file.WriteText( m_tabs + cuT( "\tmip_filter " ) + l_mapInterpolationModes[p_sampler.GetInterpolationMode( eINTERPOLATION_FILTER_MIP )] + cuT( "\n" ) ) > 0;
			}

			if ( l_return && p_sampler.GetWrappingMode( eTEXTURE_UVW_U ) != eWRAP_MODE_COUNT )
			{
				l_return = p_file.WriteText( m_tabs + cuT( "\tu_wrap_mode " ) + l_mapWrappingModes[p_sampler.GetWrappingMode( eTEXTURE_UVW_U )] + cuT( "\n" ) ) > 0;
			}

			if ( l_return && p_sampler.GetWrappingMode( eTEXTURE_UVW_V ) != eWRAP_MODE_COUNT )
			{
				l_return = p_file.WriteText( m_tabs + cuT( "\tv_wrap_mode " ) + l_mapWrappingModes[p_sampler.GetWrappingMode( eTEXTURE_UVW_V )] + cuT( "\n" ) ) > 0;
			}

			if ( l_return && p_sampler.GetWrappingMode( eTEXTURE_UVW_W ) != eWRAP_MODE_COUNT )
			{
				l_return = p_file.WriteText( m_tabs + cuT( "\tw_wrap_mode " ) + l_mapWrappingModes[p_sampler.GetWrappingMode( eTEXTURE_UVW_W )] + cuT( "\n" ) ) > 0;
			}

			if ( l_return )
			{
				l_return = p_file.Print( 256, cuT( "%s\tmin_lod %.2f\n" ), m_tabs.c_str(), p_sampler.GetMinLod() ) > 0;
			}

			if ( l_return )
			{
				l_return = p_file.Print( 256, cuT( "%s\tmax_lod %.2f\n" ), m_tabs.c_str(), p_sampler.GetMaxLod() ) > 0;
			}

			if ( l_return )
			{
				l_return = p_file.Print( 256, cuT( "%s\tlod_bias %.2f\n" ), m_tabs.c_str(), p_sampler.GetLodBias() ) > 0;
			}

			if ( l_return )
			{
				l_return = p_file.Print( 256, cuT( "%s\tborder_colour " ), m_tabs.c_str() ) > 0
					&& Colour::TextLoader( String() )( p_sampler.GetBorderColour(), p_file )
					&& p_file.WriteText( cuT( "\n" ) ) > 0;
			}

			if ( l_return )
			{
				l_return = p_file.Print( 256, cuT( "%s\tmax_anisotropy %.2f\n" ), m_tabs.c_str(), p_sampler.GetMaxAnisotropy() ) > 0;
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
		m_eWrapModes[eTEXTURE_UVW_U] = eWRAP_MODE_REPEAT;
		m_eWrapModes[eTEXTURE_UVW_V] = eWRAP_MODE_REPEAT;
		m_eWrapModes[eTEXTURE_UVW_W] = eWRAP_MODE_REPEAT;
		m_eInterpolationModes[eINTERPOLATION_FILTER_MIN] = eINTERPOLATION_MODE_NEAREST;
		m_eInterpolationModes[eINTERPOLATION_FILTER_MAG] = eINTERPOLATION_MODE_NEAREST;
		m_eInterpolationModes[eINTERPOLATION_FILTER_MIP] = eINTERPOLATION_MODE_UNDEFINED;
	}

	Sampler::~Sampler()
	{
	}
}
