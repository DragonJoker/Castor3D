#include "SpotLight.hpp"
#include "Vertex.hpp"

using namespace Castor;

namespace Castor3D
{
	bool SpotLight::TextLoader::operator()( SpotLight const & p_light, TextFile & p_file )
	{
		bool l_bReturn = LightCategory::TextLoader()( p_light, p_file );

		if ( l_bReturn )
		{
			l_bReturn = p_file.Print( 256, cuT( "\t\tattenuation " ) ) > 0 && Point3f::TextLoader()( p_light.GetAttenuation(), p_file ) && p_file.WriteText( cuT( "\n" ) ) > 0;
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.Print( 256, cuT( "\t\texponent %f\n" ), p_light.GetExponent() ) > 0;
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.Print( 256, cuT( "\t\tcut_off %f\n" ), p_light.GetCutOff() ) > 0;
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( cuT( "\t}\n" ) ) > 0;
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	SpotLight::BinaryParser::BinaryParser( Path const & p_path )
		: LightCategory::BinaryParser( p_path )
	{
	}

	bool SpotLight::BinaryParser::Fill( SpotLight const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_LIGHT );

		if ( l_bReturn )
		{
			l_bReturn = LightCategory::BinaryParser( m_path ).Fill( p_obj, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetAttenuation(), eCHUNK_TYPE_LIGHT_ATTENUATION, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetExponent(), eCHUNK_TYPE_LIGHT_EXPONENT, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetCutOff(), eCHUNK_TYPE_LIGHT_CUTOFF, l_chunk );
		}

		if ( l_bReturn )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_bReturn;
	}

	bool SpotLight::BinaryParser::Parse( SpotLight & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		float l_value;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_bReturn = p_chunk.GetSubChunk( l_chunk );

			if ( l_bReturn )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_LIGHT_ATTENUATION:
					l_bReturn = DoParseChunk( p_obj.GetAttenuation(), l_chunk );
					break;

				case eCHUNK_TYPE_LIGHT_EXPONENT:
					l_bReturn = DoParseChunk( l_value, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetExponent( l_value );
					}

					break;

				case eCHUNK_TYPE_LIGHT_CUTOFF:
					l_bReturn = DoParseChunk( l_value, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetCutOff( l_value );
					}

					break;

				default:
					l_bReturn = LightCategory::BinaryParser( m_path ).Parse( p_obj, l_chunk );
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

	//*************************************************************************************************

	SpotLight::SpotLight()
		: LightCategory( eLIGHT_TYPE_SPOT )
		, m_attenuation( 1, 0, 0 )
		, m_exponent( 0 )
		, m_cutOff( 180 )
	{
	}

	SpotLight::~SpotLight()
	{
	}

	LightCategorySPtr SpotLight::Create()
	{
		return std::make_shared< SpotLight >();
	}

	void SpotLight::SetPosition( Castor::Point3r const & p_ptPosition )
	{
		LightCategory::SetPositionType( Castor::Point4f( p_ptPosition[0], p_ptPosition[1], p_ptPosition[2], 2.0f ) );
	}

	Castor::Point3f SpotLight::GetPosition()const
	{
		Point4f const & l_position = LightCategory::GetPositionType();
		return Point3f( l_position[0], l_position[1], l_position[2] );
	}

	void SpotLight::SetAttenuation( Point3f const & p_attenuation )
	{
		m_attenuation = p_attenuation;
	}

	void SpotLight::SetExponent( float p_exponent )
	{
		m_exponent = p_exponent;
	}

	void SpotLight::SetCutOff( float p_cutOff )
	{
		m_cutOff = p_cutOff;
	}
}
