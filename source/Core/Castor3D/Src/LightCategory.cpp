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
			l_bReturn = p_file.WriteText( cuT( "\t\tambient " ) ) > 0 && Colour::TextLoader()( p_light.GetAmbient(), p_file ) && p_file.WriteText( cuT( "\n" ) ) > 0;
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( cuT( "\t\tdiffuse " ) ) > 0 && Colour::TextLoader()( p_light.GetDiffuse(), p_file ) && p_file.WriteText( cuT( "\n" ) ) > 0;
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( cuT( "\t\tspecular " ) ) > 0 && Colour::TextLoader()( p_light.GetSpecular(), p_file ) && p_file.WriteText( cuT( "\n" ) ) > 0;
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
		Colour l_colour;

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
		, m_ambient( Colour::from_rgba( 0x000000FF ) )
		, m_diffuse( Colour::from_rgba( 0x000000FF ) )
		, m_specular( Colour::from_rgba( 0xFFFFFFFF ) )
		, m_ptPositionType( 0.0f, 0.0f, 1.0f, 0.0f )
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

	void LightCategory::SetAmbient( Colour const & p_ambient )
	{
		m_ambient.red() = p_ambient.red();
		m_ambient.green() = p_ambient.green();
		m_ambient.blue() = p_ambient.blue();
	}

	void LightCategory::SetDiffuse( Colour const & p_diffuse )
	{
		m_diffuse.red() = p_diffuse.red();
		m_diffuse.green() = p_diffuse.green();
		m_diffuse.blue() = p_diffuse.blue();
	}

	void LightCategory::SetSpecular( Colour const & p_specular )
	{
		m_specular.red() = p_specular.red();
		m_specular.green() = p_specular.green();
		m_specular.blue() = p_specular.blue();
	}
}
