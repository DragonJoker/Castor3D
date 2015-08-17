#include "Light.hpp"

#include "DirectionalLight.hpp"
#include "Engine.hpp"
#include "LightFactory.hpp"
#include "Pipeline.hpp"
#include "PointLight.hpp"
#include "RenderSystem.hpp"
#include "Scene.hpp"
#include "SceneNode.hpp"
#include "SpotLight.hpp"

#include <Factory.hpp>

using namespace Castor;

namespace Castor3D
{
	Light::Light( LightFactory & p_factory, SceneSPtr p_pScene, eLIGHT_TYPE p_eLightType, SceneNodeSPtr p_pNode, String const & p_name )
		: MovableObject( p_pScene, p_pNode, p_name, eMOVABLE_TYPE_LIGHT )
		, m_pEngine( p_pScene->GetEngine() )
		, m_enabled( false )
	{
		m_pCategory = p_factory.Create( p_eLightType );
		m_pCategory->SetLight( this );

		if ( p_pNode )
		{
			m_pCategory->SetPositionType( Point4f( p_pNode->GetPosition()[0], p_pNode->GetPosition()[1], p_pNode->GetPosition()[2], real( 0.0 ) ) );
		}
	}

	Light::Light( LightFactory & p_factory, SceneSPtr p_pScene, eLIGHT_TYPE p_eLightType )
		:	Light( p_factory, p_pScene, p_eLightType, nullptr, String() )
	{
	}

	Light::~Light()
	{
	}

	void Light::Render()
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
	}

	void Light::EndRender()
	{
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
