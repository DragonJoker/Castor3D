#include "Sampler.hpp"
#include "RenderTarget.hpp"

using namespace Castor;

namespace Castor3D
{
	Sampler::TextLoader::TextLoader( File::eENCODING_MODE p_eEncodingMode )
		:	Loader< Sampler, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
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
				l_mapInterpolationModes[ eINTERPOLATION_MODE_ANISOTROPIC] = cuT( "anisotropic" );
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

			// Not supported yet by SceneFileParser
			//if( l_return )
			//{
			//	l_return = p_file.WriteText( cuT( "\tmip_filter " ) + l_mapInterpolationModes[p_sampler.GetInterpolationMode( eINTERPOLATION_FILTER_MIP )] + cuT( "\n" ) ) > 0;
			//}

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

	//*************************************************************************************************

	Sampler::BinaryParser::BinaryParser( Path const & p_path )
		:	Castor3D::BinaryParser< Sampler >( p_path )
	{
	}

	bool Sampler::BinaryParser::Fill( Sampler const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_SAMPLER );

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetName(), eCHUNK_TYPE_NAME, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetInterpolationMode( eINTERPOLATION_FILTER_MIN ), eCHUNK_TYPE_SAMPLER_MINFILTER, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetInterpolationMode( eINTERPOLATION_FILTER_MAG ), eCHUNK_TYPE_SAMPLER_MAGFILTER, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetInterpolationMode( eINTERPOLATION_FILTER_MIP ), eCHUNK_TYPE_SAMPLER_MIPFILTER, l_chunk );
		}

		if ( l_bReturn )
		{
			float l_float = float( p_obj.GetMinLod() );
			l_bReturn = DoFillChunk( l_float, eCHUNK_TYPE_SAMPLER_MINLOD, l_chunk );
		}

		if ( l_bReturn )
		{
			float l_float = float( p_obj.GetMaxLod() );
			l_bReturn = DoFillChunk( l_float, eCHUNK_TYPE_SAMPLER_MAXLOD, l_chunk );
		}

		if ( l_bReturn )
		{
			float l_float = float( p_obj.GetLodBias() );
			l_bReturn = DoFillChunk( l_float, eCHUNK_TYPE_SAMPLER_LODBIAS, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetWrappingMode( eTEXTURE_UVW_U ), eCHUNK_TYPE_SAMPLER_UWRAP, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetWrappingMode( eTEXTURE_UVW_V ), eCHUNK_TYPE_SAMPLER_VWRAP, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetWrappingMode( eTEXTURE_UVW_W ), eCHUNK_TYPE_SAMPLER_WWRAP, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetBorderColour(), eCHUNK_TYPE_SAMPLER_COLOUR, l_chunk );
		}

		if ( l_bReturn )
		{
			float l_float = float( p_obj.GetMaxAnisotropy() );
			l_bReturn = DoFillChunk( l_float, eCHUNK_TYPE_SAMPLER_MAXANISOTROPY, l_chunk );
		}

		if ( l_bReturn )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_bReturn;
	}

	bool Sampler::BinaryParser::Parse( Sampler & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		float l_float;
		eINTERPOLATION_MODE l_mode;
		eWRAP_MODE l_wrap;
		Colour l_colour;
		String l_name;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_bReturn = p_chunk.GetSubChunk( l_chunk );

			if ( l_bReturn )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_NAME:
					l_bReturn = DoParseChunk( l_name, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetName( l_name );
					}

					break;

				case eCHUNK_TYPE_SAMPLER_MINFILTER:
					l_bReturn = DoParseChunk( l_mode, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetInterpolationMode( eINTERPOLATION_FILTER_MIN, l_mode );
					}

					break;

				case eCHUNK_TYPE_SAMPLER_MAGFILTER:
					l_bReturn = DoParseChunk( l_mode, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetInterpolationMode( eINTERPOLATION_FILTER_MAG, l_mode );
					}

					break;

				case eCHUNK_TYPE_SAMPLER_MIPFILTER:
					l_bReturn = DoParseChunk( l_mode, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetInterpolationMode( eINTERPOLATION_FILTER_MIP, l_mode );
					}

					break;

				case eCHUNK_TYPE_SAMPLER_MINLOD:
					l_bReturn = DoParseChunk( l_float, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetMinLod( l_float );
					}

					break;

				case eCHUNK_TYPE_SAMPLER_MAXLOD:
					l_bReturn = DoParseChunk( l_float, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetMaxLod( l_float );
					}

					break;

				case eCHUNK_TYPE_SAMPLER_LODBIAS:
					l_bReturn = DoParseChunk( l_float, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetLodBias( l_float );
					}

					break;

				case eCHUNK_TYPE_SAMPLER_UWRAP:
					l_bReturn = DoParseChunk( l_wrap, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetWrappingMode( eTEXTURE_UVW_U, l_wrap );
					}

					break;

				case eCHUNK_TYPE_SAMPLER_VWRAP:
					l_bReturn = DoParseChunk( l_wrap, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetWrappingMode( eTEXTURE_UVW_V, l_wrap );
					}

					break;

				case eCHUNK_TYPE_SAMPLER_WWRAP:
					l_bReturn = DoParseChunk( l_wrap, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetWrappingMode( eTEXTURE_UVW_W, l_wrap );
					}

					break;

				case eCHUNK_TYPE_SAMPLER_COLOUR:
					l_bReturn = DoParseChunk( l_colour, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetBorderColour( l_colour );
					}

					break;

				case eCHUNK_TYPE_SAMPLER_MAXANISOTROPY:
					l_bReturn = DoParseChunk( l_float, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetMaxAnisotropy( l_float );
					}

					break;

				default:
					l_bReturn = false;
					break;
				}
			}

			if ( !l_bReturn )
			{
				p_chunk.EndParse();
			}
		}

		return l_bReturn;
	}

	//*********************************************************************************************

	Sampler::Sampler( Engine * p_pEngine, String const & p_name )
		: m_pEngine( p_pEngine )
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
