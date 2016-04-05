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
	Light::Light( String const & p_name, Scene & p_scene, SceneNodeSPtr p_node, LightFactory & p_factory, eLIGHT_TYPE p_lightType )
		: MovableObject( p_name, p_scene, eMOVABLE_TYPE_LIGHT, p_node )
		, m_enabled( false )
	{
		m_pCategory = p_factory.Create( p_lightType );
		m_pCategory->SetLight( this );

		if ( p_node )
		{
			m_pCategory->SetPositionType( Point4f( p_node->GetPosition()[0], p_node->GetPosition()[1], p_node->GetPosition()[2], real( 0.0 ) ) );
		}
	}

	Light::~Light()
	{
	}

	void Light::Bind( PxBufferBase & p_texture, uint32_t p_index )
	{
		SceneNodeSPtr l_node = GetParent();

		switch ( m_pCategory->GetLightType() )
		{
		case eLIGHT_TYPE_DIRECTIONAL:
			GetDirectionalLight()->Bind( p_texture, p_index );
			break;

		case eLIGHT_TYPE_POINT:
			GetPointLight()->Bind( p_texture, p_index );
			break;

		case eLIGHT_TYPE_SPOT:
			GetSpotLight()->Bind( p_texture, p_index );
			break;
		}
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
