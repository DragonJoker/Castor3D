#include "Castor3D/Scene/Light/Light.hpp"

#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/LightFactory.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"

namespace castor3d
{
	Light::Light( castor::String const & name
		, Scene & scene
		, SceneNode & node
		, LightFactory & factory
		, LightType lightType )
		: MovableObject{ name, scene, MovableType::eLight, node }
	{
		m_category = factory.create( lightType, std::ref( *this ) );
	}

	void Light::update( CpuUpdater & updater )
	{
		m_category->update();
		onGPUChanged( *this );
		m_currentGlobalIllumination = m_shadows.globalIllumination;
		m_currentShadowCaster = m_shadowCaster;
	}

	void Light::fillBuffer( uint32_t index
		, LightBuffer::LightData & data )
	{
		m_bufferIndex = index;
		m_category->fillBuffer( data );
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
}
