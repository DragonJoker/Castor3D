#include "Light.hpp"

#include "Engine.hpp"

#include "Scene/Light/DirectionalLight.hpp"
#include "Scene/Light/LightFactory.hpp"
#include "Scene/Light/PointLight.hpp"
#include "Scene/Light/SpotLight.hpp"
#include "Scene/Scene.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	Light::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< Light >{ p_tabs }
	{
	}

	bool Light::TextWriter::operator()( Light const & p_object, TextFile & p_file )
	{
		return p_object.m_category->createTextWriter( m_tabs )->writeInto( p_file );
	}

	//*************************************************************************************************

	Light::Light( String const & p_name, Scene & p_scene, SceneNodeSPtr p_node, LightFactory & p_factory, LightType p_lightType )
		: MovableObject{ p_name, p_scene, MovableType::eLight, p_node }
	{
		m_category = p_factory.create( p_lightType, std::ref( *this ) );

		if ( p_node )
		{
			m_notifyIndex = p_node->onChanged.connect( std::bind( &Light::onNodeChanged, this, std::placeholders::_1 ) );
			onNodeChanged( *p_node );
		}
	}

	Light::~Light()
	{
	}

	void Light::update()
	{
		m_category->update();
	}

	void Light::updateShadow( Point3r const & p_target
		, Viewport & p_viewport
		, int32_t p_index )
	{
		m_category->updateShadow( p_target
			, p_viewport
			, p_index );
	}

	void Light::bind( PxBufferBase & p_texture, uint32_t p_index )
	{
		SceneNodeSPtr node = getParent();

		switch ( m_category->getLightType() )
		{
		case LightType::eDirectional:
			getDirectionalLight()->bind( p_texture, p_index );
			break;

		case LightType::ePoint:
			getPointLight()->bind( p_texture, p_index );
			break;

		case LightType::eSpot:
			getSpotLight()->bind( p_texture, p_index );
			break;
		}
	}

	void Light::attachTo( SceneNodeSPtr p_node )
	{
		MovableObject::attachTo( p_node );
		auto node = getParent();

		if ( node )
		{
			m_notifyIndex = p_node->onChanged.connect( std::bind( &Light::onNodeChanged, this, std::placeholders::_1 ) );
			onNodeChanged( *node );
		}
	}

	DirectionalLightSPtr Light::getDirectionalLight()const
	{
		REQUIRE( m_category->getLightType() == LightType::eDirectional );
		return std::static_pointer_cast< DirectionalLight >( m_category );
	}

	PointLightSPtr Light::getPointLight()const
	{
		REQUIRE( m_category->getLightType() == LightType::ePoint );
		return std::static_pointer_cast< PointLight >( m_category );
	}

	SpotLightSPtr Light::getSpotLight()const
	{
		REQUIRE( m_category->getLightType() == LightType::eSpot );
		return std::static_pointer_cast< SpotLight >( m_category );
	}

	void Light::onNodeChanged( SceneNode const & p_node )
	{
		m_category->updateNode( p_node );
	}
}
