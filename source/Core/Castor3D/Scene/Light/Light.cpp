#include "Castor3D/Scene/Light/Light.hpp"

#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/LightFactory.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"

CU_ImplementSmartPtr( castor3d, Light )

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
		, castor::Point4f * data )
	{
		m_bufferIndex = index;
		m_category->fillBuffer( data );
	}

	DirectionalLightRPtr Light::getDirectionalLight()const
	{
		CU_Require( m_category->getLightType() == LightType::eDirectional );
		return static_cast< DirectionalLight * >( m_category.get() );
	}

	PointLightRPtr Light::getPointLight()const
	{
		CU_Require( m_category->getLightType() == LightType::ePoint );
		return static_cast< PointLight * >( m_category.get() );
	}

	SpotLightRPtr Light::getSpotLight()const
	{
		CU_Require( m_category->getLightType() == LightType::eSpot );
		return static_cast< SpotLight * >( m_category.get() );
	}
}
