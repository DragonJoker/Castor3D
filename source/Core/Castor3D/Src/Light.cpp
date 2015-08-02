#include "Light.hpp"
#include "LightFactory.hpp"
#include "Scene.hpp"
#include "SceneNode.hpp"
#include "DirectionalLight.hpp"
#include "PointLight.hpp"
#include "SpotLight.hpp"
#include "Engine.hpp"
#include "RenderSystem.hpp"
#include "Pipeline.hpp"

#include <Factory.hpp>

using namespace Castor;

namespace Castor3D
{
	Light::Light( LightFactory & p_factory, SceneSPtr p_pScene, eLIGHT_TYPE p_eLightType, SceneNodeSPtr p_pNode, String const & p_name )
		:	MovableObject( p_pScene, p_pNode, p_name, eMOVABLE_TYPE_LIGHT )
		,	Renderable< Light, LightRenderer >( p_pScene->GetEngine() )
		,	m_enabled( false )
	{
		DoCreateRenderer( this );
		m_pCategory = p_factory.Create( p_eLightType );
		m_pCategory->SetLight( this );

		if ( p_pNode )
		{
			m_pCategory->SetPositionType( Point4f( p_pNode->GetPosition()[0], p_pNode->GetPosition()[1], p_pNode->GetPosition()[2], real( 0.0 ) ) );
		}

		GetRenderer()->Initialise();
	}

	Light::Light( LightFactory & p_factory, SceneSPtr p_pScene, eLIGHT_TYPE p_eLightType )
		:	Light( p_factory, p_pScene, p_eLightType, nullptr, String() )
	{
	}

	Light::~Light()
	{
	}

	void Light::Enable()
	{
		LightRendererSPtr l_renderer = GetRenderer();

		if ( l_renderer )
		{
			l_renderer->Enable();
		}
	}

	void Light::Disable()
	{
		LightRendererSPtr l_renderer = GetRenderer();

		if ( l_renderer )
		{
			l_renderer->Disable();
		}
	}

	void Light::Enable( ShaderProgramBase * p_pProgram )
	{
		LightRendererSPtr l_renderer = GetRenderer();

		if ( l_renderer )
		{
			l_renderer->EnableShader( p_pProgram );
		}
	}

	void Light::Disable( ShaderProgramBase * p_pProgram )
	{
		LightRendererSPtr l_renderer = GetRenderer();

		if ( l_renderer )
		{
			l_renderer->DisableShader( p_pProgram );
		}
	}

	void Light::Render()
	{
		LightRendererSPtr l_renderer = GetRenderer();

		if ( l_renderer )
		{
			SceneNodeSPtr l_node = GetParent();

			if ( l_node )
			{
				Point3r l_position = l_node->GetDerivedPosition();

				switch ( m_pCategory->GetLightType() )
				{
				case eLIGHT_TYPE_DIRECTIONAL:
					std::static_pointer_cast< DirectionalLight >( m_pCategory )->SetDirection( l_position );
					break;

				case eLIGHT_TYPE_POINT:
					std::static_pointer_cast< PointLight >( m_pCategory )->SetPosition( l_position );
					break;

				case eLIGHT_TYPE_SPOT:
					std::static_pointer_cast< SpotLight >( m_pCategory )->SetPosition( l_position );
					break;
				}
			}

			m_pCategory->Render( l_renderer );
		}
	}

	void Light::EndRender()
	{
		Disable();
	}

	void Light::Render( ShaderProgramBase * p_pProgram )
	{
		LightRendererSPtr l_renderer = GetRenderer();

		if ( l_renderer )
		{
			SceneNodeSPtr l_node = GetParent();

			if ( l_node )
			{
				Point3r l_position = l_node->GetDerivedPosition();

				switch ( m_pCategory->GetLightType() )
				{
				case eLIGHT_TYPE_DIRECTIONAL:
					std::static_pointer_cast< DirectionalLight >( m_pCategory )->SetDirection( point::get_normalised( l_position ) );
					break;

				default:
					std::static_pointer_cast< PointLight >( m_pCategory )->SetPosition( l_position );
					break;

				case eLIGHT_TYPE_SPOT:
					std::static_pointer_cast< SpotLight >( m_pCategory )->SetPosition( l_position );
					break;
				}
			}

			m_pCategory->Render( l_renderer, p_pProgram );
		}
	}

	void Light::EndRender( ShaderProgramBase * p_pProgram )
	{
		Disable( p_pProgram );
	}

	void Light::AttachTo( SceneNodeSPtr p_pNode )
	{
		Point4f l_ptPosType = GetPositionType();
		SceneNodeSPtr l_node = GetParent();

		if ( l_node )
		{
			l_node->SetPosition( Point3r( l_ptPosType[0], l_ptPosType[1], l_ptPosType[2] ) );
		}

		MovableObject::AttachTo( p_pNode );
		l_node = GetParent();

		if ( l_node )
		{
			l_ptPosType[0] = float( l_node->GetPosition()[0] );
			l_ptPosType[1] = float( l_node->GetPosition()[1] );
			l_ptPosType[2] = float( l_node->GetPosition()[2] );
		}
		else
		{
			l_ptPosType[0] = 0;
			l_ptPosType[1] = 0;
			l_ptPosType[2] = 0;
		}

		m_pCategory->SetPositionType( l_ptPosType );
	}
}
