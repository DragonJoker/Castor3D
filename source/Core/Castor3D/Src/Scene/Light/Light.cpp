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

	Light::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< Light >{ tabs }
	{
	}

	bool Light::TextWriter::operator()( Light const & object, TextFile & file )
	{
		return object.m_category->createTextWriter( m_tabs )->writeInto( file );
	}

	//*************************************************************************************************

	Light::Light( String const & name
		, Scene & scene
		, SceneNodeSPtr node
		, LightFactory & factory
		, LightType lightType )
		: MovableObject{ name, scene, MovableType::eLight, node }
	{
		m_category = factory.create( lightType, std::ref( *this ) );

		if ( node )
		{
			m_notifyIndex = node->onChanged.connect( std::bind( &Light::onNodeChanged, this, std::placeholders::_1 ) );
			onNodeChanged( *node );
		}
	}

	Light::~Light()
	{
	}

	void Light::update()
	{
		m_category->update();
	}

	void Light::updateShadow( Point3r const & target
		, Viewport & viewport
		, int32_t index )
	{
		m_category->updateShadow( target
			, viewport
			, index );
	}

	void Light::bind( Point4f * buffer )
	{
		m_category->bind( buffer );
	}

	void Light::attachTo( SceneNodeSPtr node )
	{
		MovableObject::attachTo( node );
		auto parent = getParent();

		if ( parent )
		{
			m_notifyIndex = node->onChanged.connect( std::bind( &Light::onNodeChanged, this, std::placeholders::_1 ) );
			onNodeChanged( *parent );
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

	void Light::onNodeChanged( SceneNode const & node )
	{
		m_category->updateNode( node );
	}
}
