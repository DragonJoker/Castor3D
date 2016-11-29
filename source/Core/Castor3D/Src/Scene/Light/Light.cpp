#include "Light.hpp"

#include "Engine.hpp"

#include "DirectionalLight.hpp"
#include "LightFactory.hpp"
#include "PointLight.hpp"
#include "SpotLight.hpp"

#include "Event/Frame/InitialiseEvent.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/Viewport.hpp"
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
		: MovableObject{ p_name, p_scene, MovableType::eLight, p_node }
	{
		m_category = p_factory.Create( p_lightType, std::ref( *this ) );

		if ( p_node )
		{
			m_notifyIndex = p_node->RegisterObject( std::bind( &Light::OnNodeChanged, this, std::placeholders::_1 ) );
			OnNodeChanged( *p_node );
		}
	}

	Light::~Light()
	{
	}

	void Light::Update( Point3r const & p_target )
	{
		m_category->Update( p_target );
	}

	void Light::Bind( PxBufferBase & p_texture, uint32_t p_index )
	{
		SceneNodeSPtr l_node = GetParent();

		switch ( m_category->GetLightType() )
		{
		case LightType::eDirectional:
			GetDirectionalLight()->Bind( p_texture, p_index );
			break;

		case LightType::ePoint:
			GetPointLight()->Bind( p_texture, p_index );
			break;

		case LightType::eSpot:
			GetSpotLight()->Bind( p_texture, p_index );
			break;
		}
	}

	void Light::AttachTo( SceneNodeSPtr p_node )
	{
		MovableObject::AttachTo( p_node );
		auto l_node = GetParent();

		if ( l_node )
		{
			m_notifyIndex = l_node->RegisterObject( std::bind( &Light::OnNodeChanged, this, std::placeholders::_1 ) );
			OnNodeChanged( *l_node );
		}
	}

	DirectionalLightSPtr Light::GetDirectionalLight()const
	{
		DirectionalLightSPtr l_return;

		if ( m_category->GetLightType() == LightType::eDirectional )
		{
			l_return = std::static_pointer_cast< DirectionalLight >( m_category );
		}

		return l_return;
	}

	PointLightSPtr Light::GetPointLight()const
	{
		PointLightSPtr l_return;

		if ( m_category->GetLightType() == LightType::ePoint )
		{
			l_return = std::static_pointer_cast< PointLight >( m_category );
		}

		return l_return;
	}

	SpotLightSPtr Light::GetSpotLight()const
	{
		SpotLightSPtr l_return;

		if ( m_category->GetLightType() == LightType::eSpot )
		{
			l_return = std::static_pointer_cast< SpotLight >( m_category );
		}

		return l_return;
	}

	void Light::OnNodeChanged( SceneNode const & p_node )
	{
		m_category->UpdateNode( p_node );
	}
}
