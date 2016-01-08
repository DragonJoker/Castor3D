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
	Light::Light( LightFactory & p_factory, SceneSPtr p_scene, eLIGHT_TYPE p_eLightType, SceneNodeSPtr p_node, String const & p_name )
		: MovableObject( p_scene, p_node, p_name, eMOVABLE_TYPE_LIGHT )
		, m_enabled( false )
	{
		m_pCategory = p_factory.Create( p_eLightType );
		m_pCategory->SetLight( this );

		if ( p_node )
		{
			m_pCategory->SetPositionType( Point4f( p_node->GetPosition()[0], p_node->GetPosition()[1], p_node->GetPosition()[2], real( 0.0 ) ) );
		}
	}

	Light::Light( LightFactory & p_factory, SceneSPtr p_scene, eLIGHT_TYPE p_eLightType )
		:	Light( p_factory, p_scene, p_eLightType, nullptr, String() )
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
				std::static_pointer_cast< DirectionalLight >( m_pCategory )->SetDirection( point::get_normalised( l_position ) );
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

	void Light::AttachTo( SceneNodeSPtr p_node )
	{
		Point4f l_ptPosType = GetPositionType();
		SceneNodeSPtr l_node = GetParent();

		if ( l_node )
		{
			l_node->SetPosition( Point3r( l_ptPosType[0], l_ptPosType[1], l_ptPosType[2] ) );
		}

		MovableObject::AttachTo( p_node );
		l_node = GetParent();

		if ( l_node )
		{
			m_notifyIndex = l_node->RegisterObject( std::bind( &Light::OnNodeChanged, this ) );
			OnNodeChanged();
		}
		else
		{
			l_ptPosType[0] = 0;
			l_ptPosType[1] = 0;
			l_ptPosType[2] = 0;
			m_pCategory->SetPositionType( l_ptPosType );
		}
	}

	DirectionalLightSPtr Light::GetDirectionalLight()const
	{
		DirectionalLightSPtr l_return;

		if ( m_pCategory->GetLightType() == eLIGHT_TYPE_DIRECTIONAL )
		{
			l_return = std::static_pointer_cast< DirectionalLight >( m_pCategory );
		}

		return l_return;
	}

	PointLightSPtr Light::GetPointLight()const
	{
		PointLightSPtr l_return;

		if ( m_pCategory->GetLightType() == eLIGHT_TYPE_POINT )
		{
			l_return = std::static_pointer_cast< PointLight >( m_pCategory );
		}

		return l_return;
	}

	SpotLightSPtr Light::GetSpotLight()const
	{
		SpotLightSPtr l_return;

		if ( m_pCategory->GetLightType() == eLIGHT_TYPE_SPOT )
		{
			l_return = std::static_pointer_cast< SpotLight >( m_pCategory );
		}

		return l_return;
	}

	void Light::OnNodeChanged()
	{
		SceneNodeSPtr l_node = GetParent();
		Point4f l_ptPosType = GetPositionType();
		l_ptPosType[0] = float( l_node->GetDerivedPosition()[0] );
		l_ptPosType[1] = float( l_node->GetDerivedPosition()[1] );
		l_ptPosType[2] = float( l_node->GetDerivedPosition()[2] );
		m_pCategory->SetPositionType( l_ptPosType );
	}
}
