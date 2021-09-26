#include "Castor3D/Scene/Light/Light.hpp"

#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/LightFactory.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"

using namespace castor;

namespace castor3d
{
	Light::Light( String const & name
		, Scene & scene
		, SceneNode & node
		, LightFactory & factory
		, LightType lightType )
		: MovableObject{ name, scene, MovableType::eLight, node }
	{
		m_category = factory.create( lightType, std::ref( *this ) );
		m_notifyIndex = node.onChanged.connect( [this]( SceneNode const & pnode )
			{
				onNodeChanged( pnode );
			} );
		onNodeChanged( node );
		getScene()->registerLight( *this );
	}

	void Light::update( CpuUpdater & updater )
	{
		m_category->update();
		m_dirty = false;
		m_currentGlobalIllumination = m_shadows.globalIllumination;
		m_currentShadowCaster = m_shadowCaster;
	}

	void Light::bind( Point4f * buffer )const
	{
		m_category->bind( buffer );
	}

	void Light::attachTo( SceneNode & node )
	{
		MovableObject::attachTo( node );
		auto parent = getParent();

		if ( parent )
		{
			m_notifyIndex = node.onChanged.connect( [this]( SceneNode const & pnode )
				{
					onNodeChanged( pnode );
				} );
			onNodeChanged( *parent );
			m_dirty = true;
		}
	}

	DirectionalLightSPtr Light::getDirectionalLight()const
	{
		CU_Require( m_category->getLightType() == LightType::eDirectional );
		return std::static_pointer_cast< DirectionalLight >( m_category );
	}

	PointLightSPtr Light::getPointLight()const
	{
		CU_Require( m_category->getLightType() == LightType::ePoint );
		return std::static_pointer_cast< PointLight >( m_category );
	}

	SpotLightSPtr Light::getSpotLight()const
	{
		CU_Require( m_category->getLightType() == LightType::eSpot );
		return std::static_pointer_cast< SpotLight >( m_category );
	}

	void Light::onNodeChanged( SceneNode const & node )
	{
		m_dirty = true;
		m_category->updateNode( node );
	}
}
