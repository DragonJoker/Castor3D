#include "PointLight.hpp"
#include "Vertex.hpp"

using namespace Castor;

namespace Castor3D
{
	bool PointLight::TextLoader::operator()( PointLight const & p_light, TextFile & p_file )
	{
		bool l_return = LightCategory::TextLoader()( p_light, p_file );

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "\t\tattenuation " ) ) > 0 && Point3f::TextLoader()( p_light.GetAttenuation(), p_file ) && p_file.WriteText( cuT( "\n" ) ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "\t}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	PointLight::BinaryParser::BinaryParser( Path const & p_path )
		: LightCategory::BinaryParser( p_path )
	{
	}

	bool PointLight::BinaryParser::Fill( PointLight const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_LIGHT );

		if ( l_return )
		{
			l_return = LightCategory::BinaryParser( m_path ).Fill( p_obj, l_chunk );
		}

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.GetAttenuation(), eCHUNK_TYPE_LIGHT_ATTENUATION, l_chunk );
		}

		if ( l_return )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_return;
	}

	bool PointLight::BinaryParser::Parse( PointLight & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		String l_name;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_return = p_chunk.GetSubChunk( l_chunk );

			if ( l_return )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_LIGHT_ATTENUATION:
					l_return = DoParseChunk( p_obj.GetAttenuation(), l_chunk );
					break;

				default:
					l_return = LightCategory::BinaryParser( m_path ).Parse( p_obj, l_chunk );
					break;
				}
			}

			if ( !l_return )
			{
				p_chunk.EndParse();
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	PointLight::PointLight()
		: LightCategory( eLIGHT_TYPE_POINT )
		, m_attenuation( 1.0f, 0.0f, 0.0f )
	{
	}

	PointLight::~PointLight()
	{
	}

	LightCategorySPtr PointLight::Create()
	{
		return std::make_shared< PointLight >();
	}

	void PointLight::SetPosition( Castor::Point3r const & p_ptPosition )
	{
		LightCategory::SetPositionType( Castor::Point4f( p_ptPosition[0], p_ptPosition[1], p_ptPosition[2], 1.0f ) );
	}

	Castor::Point3f PointLight::GetPosition()const
	{
		Point4f const & l_position = LightCategory::GetPositionType();
		return Point3f( l_position[0], l_position[1], l_position[2] );
	}

	void PointLight::SetAttenuation( Point3f const & p_attenuation )
	{
		m_attenuation = p_attenuation;
	}
}
