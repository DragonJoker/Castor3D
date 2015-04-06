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
	Light::Light( SceneSPtr p_pScene, LightFactory & p_factory, eLIGHT_TYPE p_eLightType )
		:	MovableObject( p_pScene, eMOVABLE_TYPE_LIGHT )
		,	Renderable< Light, LightRenderer >( p_pScene->GetEngine() )
		,	m_enabled( false )
	{
		m_pCategory = p_factory.Create( p_eLightType );
		m_pCategory->SetLight( this );
		GetRenderer()->Initialise();
	}

	Light::Light( LightFactory & p_factory, SceneSPtr p_pScene, SceneNodeSPtr p_pNode, String const & p_name, eLIGHT_TYPE p_eLightType )
		:	MovableObject( p_pScene, p_pNode, p_name, eMOVABLE_TYPE_LIGHT )
		,	Renderable< Light, LightRenderer >( p_pScene->GetEngine() )
		,	m_enabled( false )
	{
		m_pCategory = p_factory.Create( p_eLightType );
		m_pCategory->SetLight( this );
		m_pCategory->SetPositionType( Point4f( p_pNode->GetPosition()[0], p_pNode->GetPosition()[1], p_pNode->GetPosition()[2], real( 0.0 ) ) );
		GetRenderer()->Initialise();
	}

	Light::~Light()
	{
	}

	void Light::Enable()
	{
		if ( ! m_pRenderer.expired() )
		{
			m_pRenderer.lock()->Enable();
		}
	}

	void Light::Disable()
	{
		if ( ! m_pRenderer.expired() )
		{
			m_pRenderer.lock()->Disable();
		}
	}

	void Light::Enable( ShaderProgramBase * p_pProgram )
	{
		if ( ! m_pRenderer.expired() )
		{
			m_pRenderer.lock()->EnableShader( p_pProgram );
		}
	}

	void Light::Disable( ShaderProgramBase * p_pProgram )
	{
		if ( ! m_pRenderer.expired() )
		{
			m_pRenderer.lock()->DisableShader( p_pProgram );
		}
	}

	void Light::Render()
	{
		if ( ! m_pRenderer.expired() )
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

			m_pCategory->Render( m_pRenderer.lock() );
		}
	}

	void Light::EndRender()
	{
		Disable();
	}

	void Light::Render( ShaderProgramBase * p_pProgram )
	{
		if ( ! m_pRenderer.expired() )
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

			m_pCategory->Render( m_pRenderer.lock(), p_pProgram );
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
