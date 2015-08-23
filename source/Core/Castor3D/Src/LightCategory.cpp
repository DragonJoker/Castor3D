#include "LightCategory.hpp"
#include "Light.hpp"

#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	LightCategory::TextLoader::TextLoader( File::eENCODING_MODE p_eEncodingMode )
		: Loader< LightCategory, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
	{
	}

	bool LightCategory::TextLoader::operator()( LightCategory const & p_light, TextFile & p_file )
	{
		Logger::LogInfo( cuT( "Writing Light " ) + p_light.GetLight()->GetName() );
		bool l_bReturn = p_file.WriteText( cuT( "\tlight \"" ) + p_light.GetLight()->GetName() + cuT( "\"\n\t{\n" ) ) > 0;

		if ( l_bReturn )
		{
			l_bReturn = MovableObject::TextLoader()( *p_light.GetLight(), p_file );
		}

		if ( l_bReturn )
		{
			switch ( p_light.GetLightType() )
			{
			case eLIGHT_TYPE_DIRECTIONAL:
				l_bReturn = p_file.WriteText( cuT( "\t\ttype directional\n" ) ) > 0;
				break;

			case eLIGHT_TYPE_POINT:
				l_bReturn = p_file.WriteText( cuT( "\t\ttype point_light\n" ) ) > 0;
				break;

			case eLIGHT_TYPE_SPOT:
				l_bReturn = p_file.WriteText( cuT( "\t\ttype spot_light\n" ) ) > 0;
				break;
			}
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( cuT( "\t\tambient " ) ) > 0 && Point4f::TextLoader()( p_light.GetAmbient(), p_file ) && p_file.WriteText( cuT( "\n" ) ) > 0;
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( cuT( "\t\tdiffuse " ) ) > 0 && Point4f::TextLoader()( p_light.GetDiffuse(), p_file ) && p_file.WriteText( cuT( "\n" ) ) > 0;
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( cuT( "\t\tspecular " ) ) > 0 && Point4f::TextLoader()( p_light.GetSpecular(), p_file ) && p_file.WriteText( cuT( "\n" ) ) > 0;
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	LightCategory::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< LightCategory >( p_path )
	{
	}

	bool LightCategory::BinaryParser::Fill( LightCategory const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;

		if ( l_bReturn )
		{
			l_bReturn = MovableObject::BinaryParser( m_path ).Fill( *p_obj.GetLight(), p_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetLightType(), eCHUNK_TYPE_LIGHT_TYPE, p_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetAmbient(), eCHUNK_TYPE_LIGHT_AMBIENT, p_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetDiffuse(), eCHUNK_TYPE_LIGHT_DIFFUSE, p_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetSpecular(), eCHUNK_TYPE_LIGHT_SPECULAR, p_chunk );
		}

		return l_bReturn;
	}

	bool LightCategory::BinaryParser::Parse( LightCategory & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		Point4f l_colour;

		switch ( p_chunk.GetChunkType() )
		{
		case eCHUNK_TYPE_MOVABLE_NODE:
			l_bReturn = MovableObject::BinaryParser( m_path ).Parse( *p_obj.GetLight(), p_chunk );
			break;

		case eCHUNK_TYPE_LIGHT_AMBIENT:
			l_bReturn = DoParseChunk( l_colour, p_chunk );

			if ( l_bReturn )
			{
				p_obj.SetAmbient( l_colour );
			}

			break;

		case eCHUNK_TYPE_LIGHT_DIFFUSE:
			l_bReturn = DoParseChunk( l_colour, p_chunk );

			if ( l_bReturn )
			{
				p_obj.SetDiffuse( l_colour );
			}

			break;

		case eCHUNK_TYPE_LIGHT_SPECULAR:
			l_bReturn = DoParseChunk( l_colour, p_chunk );

			if ( l_bReturn )
			{
				p_obj.SetSpecular( l_colour );
			}

			break;
		}

		if ( !l_bReturn )
		{
			p_chunk.EndParse();
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	LightCategory::LightCategory( eLIGHT_TYPE p_eLightType )
		: m_eLightType( p_eLightType )
		, m_ambient( 0.0, 0.0, 0.0, 1.0 )
		, m_diffuse( 0.0, 0.0, 0.0, 1.0 )
		, m_specular( 1.0, 1.0, 1.0, 1.0 )
		, m_ptPositionType( 0.0, 0.0, 1.0, 0.0 )
	{
		switch ( p_eLightType )
		{
		case eLIGHT_TYPE_DIRECTIONAL:
			m_ptPositionType[3] = 0.0f;

		default:
			m_ptPositionType[3] = 1.0f;
		}
	}

	LightCategory::~LightCategory()
	{
	}

	void LightCategory::SetAmbient( Point4f const & p_ambient )
	{
		m_ambient = p_ambient;
	}

	void LightCategory::SetDiffuse( Point4f const & p_diffuse )
	{
		m_diffuse = p_diffuse;
	}

	void LightCategory::SetSpecular( Point4f const & p_specular )
	{
		m_specular = p_specular;
	}
}
