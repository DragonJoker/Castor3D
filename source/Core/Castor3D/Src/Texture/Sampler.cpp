#include "Sampler.hpp"

#include "Render/RenderTarget.hpp"

using namespace Castor;

namespace Castor3D
{
	Sampler::TextLoader::TextLoader( File::eENCODING_MODE p_encodingMode )
		:	Loader< Sampler, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_encodingMode )
	{
	}

	bool Sampler::TextLoader::operator()( Sampler const & p_sampler, TextFile & p_file )
	{
		bool l_return = true;

		if ( p_sampler.GetName() != cuT( "LightsSampler" ) && p_sampler.GetName() != RenderTarget::DefaultSamplerName )
		{
			typedef std::map< uint32_t, String > StrUIntMap;
			static StrUIntMap l_mapInterpolationModes;
			static StrUIntMap l_mapWrappingModes;

			if ( l_mapInterpolationModes.empty() )
			{
				l_mapInterpolationModes[ eINTERPOLATION_MODE_NEAREST] = cuT( "nearest" );
				l_mapInterpolationModes[ eINTERPOLATION_MODE_LINEAR] = cuT( "linear" );
			}

			if ( l_mapWrappingModes.empty() )
			{
				l_mapWrappingModes[ eWRAP_MODE_REPEAT] = cuT( "repeat" );
				l_mapWrappingModes[ eWRAP_MODE_MIRRORED_REPEAT] = cuT( "mirrored_repeat" );
				l_mapWrappingModes[ eWRAP_MODE_CLAMP_TO_BORDER] = cuT( "clamp_to_border" );
				l_mapWrappingModes[ eWRAP_MODE_CLAMP_TO_EDGE] = cuT( "clamp_to_edge" );
			}

			l_return = p_file.WriteText( cuT( "sampler \"" ) + p_sampler.GetName() + cuT( "\"\n{\n" ) ) > 0;

			if ( l_return )
			{
				l_return = p_file.WriteText( cuT( "\tmin_filter " ) + l_mapInterpolationModes[p_sampler.GetInterpolationMode( eINTERPOLATION_FILTER_MIN )] + cuT( "\n" ) ) > 0;
			}

			if ( l_return )
			{
				l_return = p_file.WriteText( cuT( "\tmag_filter " ) + l_mapInterpolationModes[p_sampler.GetInterpolationMode( eINTERPOLATION_FILTER_MAG )] + cuT( "\n" ) ) > 0;
			}

			if( l_return )
			{
				l_return = p_file.WriteText( cuT( "\tmip_filter " ) + l_mapInterpolationModes[p_sampler.GetInterpolationMode( eINTERPOLATION_FILTER_MIP )] + cuT( "\n" ) ) > 0;
			}

			if ( l_return )
			{
				l_return = p_file.WriteText( cuT( "\tu_wrap_mode " ) + l_mapWrappingModes[p_sampler.GetWrappingMode( eTEXTURE_UVW_U )] + cuT( "\n" ) ) > 0;
			}

			if ( l_return )
			{
				l_return = p_file.WriteText( cuT( "\tv_wrap_mode " ) + l_mapWrappingModes[p_sampler.GetWrappingMode( eTEXTURE_UVW_V )] + cuT( "\n" ) ) > 0;
			}

			if ( l_return )
			{
				l_return = p_file.WriteText( cuT( "\tw_wrap_mode " ) + l_mapWrappingModes[p_sampler.GetWrappingMode( eTEXTURE_UVW_W )] + cuT( "\n" ) ) > 0;
			}

			if ( l_return )
			{
				l_return = p_file.Print( 256, cuT( "\tmin_lod %.2f\n" ), p_sampler.GetMinLod() ) > 0;
			}

			if ( l_return )
			{
				l_return = p_file.Print( 256, cuT( "\tmax_lod %.2f\n" ), p_sampler.GetMaxLod() ) > 0;
			}

			if ( l_return )
			{
				l_return = p_file.Print( 256, cuT( "\tlod_bias %.2f\n" ), p_sampler.GetLodBias() ) > 0;
			}

			if ( l_return )
			{
				l_return = p_file.Print( 256, cuT( "\tborder_colour " ) ) > 0 && Colour::TextLoader()( p_sampler.GetBorderColour(), p_file ) && p_file.WriteText( cuT( "\n" ) ) > 0;
			}

			if ( l_return )
			{
				l_return = p_file.Print( 256, cuT( "\tmax_anisotropy %.2f\n" ), p_sampler.GetMaxAnisotropy() ) > 0;
			}

			if ( l_return )
			{
				l_return = p_file.WriteText( cuT( "}\n\n" ) ) > 0;
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
