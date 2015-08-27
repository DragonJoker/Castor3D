#include "DirectionalLight.hpp"


using namespace Castor;

namespace Castor3D
{
	bool DirectionalLight::TextLoader::operator()( DirectionalLight const & p_light, TextFile & p_file )
	{
		bool l_bReturn = LightCategory::TextLoader()( p_light, p_file );

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( cuT( "\n\t}\n" ) ) > 0;
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	DirectionalLight::BinaryParser::BinaryParser( Path const & p_path )
		:	LightCategory::BinaryParser( p_path )
	{
	}

	bool DirectionalLight::BinaryParser::Fill( DirectionalLight const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_LIGHT );

		if ( l_bReturn )
		{
			l_bReturn = LightCategory::BinaryParser( m_path ).Fill( p_obj, l_chunk );
		}

		if ( l_bReturn )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_bReturn;
	}

	bool DirectionalLight::BinaryParser::Parse( DirectionalLight & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		String l_name;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_bReturn = p_chunk.GetSubChunk( l_chunk );

			if ( l_bReturn )
			{
				l_bReturn = LightCategory::BinaryParser( m_path ).Parse( p_obj, l_chunk );
			}

			if ( !l_bReturn )
			{
				p_chunk.EndParse();
			}
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	DirectionalLight::DirectionalLight()
		:	LightCategory( eLIGHT_TYPE_DIRECTIONAL )
	{
	}

	DirectionalLight::~DirectionalLight()
	{
	}

	LightCategorySPtr DirectionalLight::Create()
	{
		return std::make_shared< DirectionalLight >();
	}

	void DirectionalLight::Render( LightRendererSPtr p_pRenderer )
	{
		if ( p_pRenderer )
		{
			p_pRenderer->Enable();
			p_pRenderer->ApplyPosition();
			p_pRenderer->ApplyAmbient();
			p_pRenderer->ApplyDiffuse();
			p_pRenderer->ApplySpecular();
			p_pRenderer->Bind();
		}
	}

	void DirectionalLight::Render( LightRendererSPtr p_pRenderer, ShaderProgramBase * p_pProgram )
	{
		if ( p_pRenderer )
		{
			p_pRenderer->ApplyPositionShader();
			p_pRenderer->ApplyAmbientShader();
			p_pRenderer->ApplyDiffuseShader();
			p_pRenderer->ApplySpecularShader();
			p_pRenderer->EnableShader( p_pProgram );
		}
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
