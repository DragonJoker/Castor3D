#include "LightCategory.hpp"
#include "Light.hpp"

#include <Logger.hpp>
#include <PixelBuffer.hpp>

using namespace Castor;

namespace Castor3D
{
	LightCategory::TextLoader::TextLoader( File::eENCODING_MODE p_encodingMode )
		: Loader< LightCategory, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_encodingMode )
	{
	}

	bool LightCategory::TextLoader::operator()( LightCategory const & p_light, TextFile & p_file )
	{
		Logger::LogInfo( cuT( "Writing Light " ) + p_light.GetLight()->GetName() );
		bool l_return = p_file.WriteText( cuT( "\tlight \"" ) + p_light.GetLight()->GetName() + cuT( "\"\n\t{\n" ) ) > 0;

		if ( l_return )
		{
			l_return = MovableObject::TextLoader()( *p_light.GetLight(), p_file );
		}

		if ( l_return )
		{
			switch ( p_light.GetLightType() )
			{
			case eLIGHT_TYPE_DIRECTIONAL:
				l_return = p_file.WriteText( cuT( "\t\ttype directional\n" ) ) > 0;
				break;

			case eLIGHT_TYPE_POINT:
				l_return = p_file.WriteText( cuT( "\t\ttype point_light\n" ) ) > 0;
				break;

			case eLIGHT_TYPE_SPOT:
				l_return = p_file.WriteText( cuT( "\t\ttype spot_light\n" ) ) > 0;
				break;
			}
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "\t\tcolour " ) ) > 0 && Point3f::TextLoader()( p_light.GetColour(), p_file ) && p_file.WriteText( cuT( "\n" ) ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "\t\tintensity " ) ) > 0 && Point3f::TextLoader()( p_light.GetIntensity(), p_file ) && p_file.WriteText( cuT( "\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	LightCategory::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< LightCategory >( p_path )
	{
	}

	bool LightCategory::BinaryParser::Fill( LightCategory const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;

		if ( l_return )
		{
			l_return = MovableObject::BinaryParser( m_path ).Fill( *p_obj.GetLight(), p_chunk );
		}

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.GetLightType(), eCHUNK_TYPE_LIGHT_TYPE, p_chunk );
		}

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.GetColour(), eCHUNK_TYPE_LIGHT_COLOUR, p_chunk );
		}

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.GetIntensity(), eCHUNK_TYPE_LIGHT_INTENSITY, p_chunk );
		}

		return l_return;
	}

	bool LightCategory::BinaryParser::Parse( LightCategory & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		Point3f l_vec3;

		switch ( p_chunk.GetChunkType() )
		{
		case eCHUNK_TYPE_MOVABLE_NODE:
			l_return = MovableObject::BinaryParser( m_path ).Parse( *p_obj.GetLight(), p_chunk );
			break;

		case eCHUNK_TYPE_LIGHT_COLOUR:
			l_return = DoParseChunk( l_vec3, p_chunk );

			if ( l_return )
			{
				p_obj.SetColour( l_vec3 );
			}

			break;

		case eCHUNK_TYPE_LIGHT_INTENSITY:
			l_return = DoParseChunk( l_vec3, p_chunk );

			if ( l_return )
			{
				p_obj.SetIntensity( l_vec3 );
			}

			break;
		}

		if ( !l_return )
		{
			p_chunk.EndParse();
		}

		return l_return;
	}

	//*************************************************************************************************

	LightCategory::LightCategory( eLIGHT_TYPE p_eLightType )
		: m_eLightType( p_eLightType )
		, m_colour( 1.0, 1.0, 1.0 )
		, m_intensity( 0.0, 1.0, 1.0 )
		, m_positionType( 0.0, 0.0, 1.0, float( p_eLightType ) )
	{
	}

	LightCategory::~LightCategory()
	{
	}

	void LightCategory::DoBindComponent( Point3f const & p_component, int p_index, int & p_offset, PxBufferBase & p_data )const
	{
		uint8_t * l_pDst = &( *p_data.get_at( p_index * 10 + p_offset++, 0 ) );
		std::memcpy( l_pDst, p_component.const_ptr(), 3 * sizeof( float ) );
	}

	void LightCategory::DoBindComponent( Point4f const & p_component, int p_index, int & p_offset, PxBufferBase & p_data )const
	{
		uint8_t * l_pDst = &( *p_data.get_at( p_index * 10 + p_offset++, 0 ) );
		std::memcpy( l_pDst, p_component.const_ptr(), 4 * sizeof( float ) );
	}

	void LightCategory::DoBindComponent( Coords4f const & p_component, int p_index, int & p_offset, PxBufferBase & p_data )const
	{
		uint8_t * l_pDst = &( *p_data.get_at( p_index * 10 + p_offset++, 0 ) );
		std::memcpy( l_pDst, p_component.const_ptr(), 4 * sizeof( float ) );
	}
}
