#include "Light.hpp"

#include "Engine.hpp"

#include "DirectionalLight.hpp"
#include "LightFactory.hpp"
#include "PointLight.hpp"
#include "SpotLight.hpp"

#include "Event/Frame/InitialiseEvent.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneNode.hpp"

#include <Design/Factory.hpp>

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	Light::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< Light >{ p_tabs }
	{
	}

	bool Light::TextWriter::operator()( Light const & p_object, TextFile & p_file )
	{
		return p_object.m_category->CreateTextWriter( m_tabs )->WriteInto( p_file );
	}

	//*************************************************************************************************

	Light::Light( String const & p_name, Scene & p_scene, SceneNodeSPtr p_node, LightFactory & p_factory, LightType p_lightType )
		: MovableObject{ p_name, p_scene, MovableType::Light, p_node }
		, m_viewport{ *p_scene.GetEngine() }
	{
		m_category = p_factory.Create( p_lightType, m_viewport );
		m_category->SetLight( this );

		if ( p_node )
		{
			m_category->SetPositionType( Point4f( p_node->GetPosition()[0], p_node->GetPosition()[1], p_node->GetPosition()[2], real( 0.0 ) ) );
		}

		GetScene()->GetEngine()->PostEvent( MakeInitialiseEvent( m_viewport ) );
	}

	Light::~Light()
	{
	}

	void Light::Update( Castor::Size const & p_size )
	{
		m_viewport.Resize( p_size );
		m_category->Update( p_size );
	}

	void Light::Bind( PxBufferBase & p_texture, uint32_t p_index )
	{
		SceneNodeSPtr l_node = GetParent();

		switch ( m_category->GetLightType() )
		{
		case LightType::Directional:
			GetDirectionalLight()->Bind( p_texture, p_index );
			break;

		case LightType::Point:
			GetPointLight()->Bind( p_texture, p_index );
			break;

		case LightType::Spot:
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
			m_notifyIndex = l_node->RegisterObject( std::bind( &Light::OnNodeChanged, this, std::placeholders::_1 ) );
			OnNodeChanged( *l_node );
		}
		else
		{
			l_ptPosType[0] = 0;
			l_ptPosType[1] = 0;
			l_ptPosType[2] = 0;
			m_category->SetPositionType( l_ptPosType );
		}
	}

	DirectionalLightSPtr Light::GetDirectionalLight()const
	{
		DirectionalLightSPtr l_return;

		if ( m_category->GetLightType() == LightType::Directional )
		{
			l_return = std::static_pointer_cast< DirectionalLight >( m_category );
		}

		return l_return;
	}

	PointLightSPtr Light::GetPointLight()const
	{
		PointLightSPtr l_return;

		if ( m_category->GetLightType() == LightType::Point )
		{
			l_return = std::static_pointer_cast< PointLight >( m_category );
		}

		return l_return;
	}

	SpotLightSPtr Light::GetSpotLight()const
	{
		SpotLightSPtr l_return;

		if ( m_category->GetLightType() == LightType::Spot )
		{
			l_return = std::static_pointer_cast< SpotLight >( m_category );
		}

		return l_return;
	}

	void Light::OnNodeChanged( SceneNode const & p_node )
	{
		auto l_posType = GetPositionType();
		auto l_derived = p_node.GetDerivedPosition();
		l_posType[0] = float( l_derived[0] );
		l_posType[1] = float( l_derived[1] );
		l_posType[2] = float( l_derived[2] );
		m_category->SetPositionType( l_posType );
	}
}
