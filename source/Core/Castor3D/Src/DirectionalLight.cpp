﻿#include "DirectionalLight.hpp"


using namespace Castor;

namespace Castor3D
{
	bool DirectionalLight::TextLoader::operator()( DirectionalLight const & p_light, TextFile & p_file )
	{
		bool l_return = LightCategory::TextLoader()( p_light, p_file );

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "\n\t}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	DirectionalLight::BinaryParser::BinaryParser( Path const & p_path )
		: LightCategory::BinaryParser( p_path )
	{
	}

	bool DirectionalLight::BinaryParser::Fill( DirectionalLight const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_LIGHT );

		if ( l_return )
		{
			l_return = LightCategory::BinaryParser( m_path ).Fill( p_obj, l_chunk );
		}

		if ( l_return )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_return;
	}

	bool DirectionalLight::BinaryParser::Parse( DirectionalLight & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		String l_name;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_return = p_chunk.GetSubChunk( l_chunk );

			if ( l_return )
			{
				l_return = LightCategory::BinaryParser( m_path ).Parse( p_obj, l_chunk );
			}

			if ( !l_return )
			{
				p_chunk.EndParse();
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	DirectionalLight::DirectionalLight()
		: LightCategory( eLIGHT_TYPE_DIRECTIONAL )
	{
	}

	DirectionalLight::~DirectionalLight()
	{
	}

	LightCategorySPtr DirectionalLight::Create()
	{
		return std::make_shared< DirectionalLight >();
	}

	void DirectionalLight::SetDirection( Castor::Point3f const & p_ptPosition )
	{
		LightCategory::SetPositionType( Castor::Point4f( p_ptPosition[0], p_ptPosition[1], p_ptPosition[2], 0.0f ) );
	}

	Castor::Point3f DirectionalLight::GetDirection()const
	{
		Point4f const & l_position = LightCategory::GetPositionType();
		return Point3f( l_position[0], l_position[1], l_position[2] );
	}
}
