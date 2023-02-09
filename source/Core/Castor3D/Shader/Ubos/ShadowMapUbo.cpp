#include "Castor3D/Shader/Ubos/ShadowMapUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		sdw::Vec4 ShadowMapData::worldToView( sdw::Vec4 const & pos )const
		{
			return lightView() * pos;
		}

		sdw::Vec4 ShadowMapData::viewToProj( sdw::Vec4 const & pos )const
		{
			return lightProjection() * pos;
		}

		sdw::Float ShadowMapData::getNormalisedDepth( sdw::Vec3 const & pos )const
		{
			return length( pos - lightPosFarPlane().xyz() ) / lightPosFarPlane().w();
		}

		DirectionalLight ShadowMapData::getDirectionalLight( Lights & lights )const
		{
			return lights.getDirectionalLight( lightOffset() );
		}

		PointLight ShadowMapData::getPointLight( Lights & lights )const
		{
			return lights.getPointLight( lightOffset() );
		}

		SpotLight ShadowMapData::getSpotLight( Lights & lights )const
		{
			return lights.getSpotLight( lightOffset() );
		}
	}

	//*********************************************************************************************

	ShadowMapUbo::ShadowMapUbo( RenderDevice const & device )
		: m_device{ device }
	{
		m_ubo = m_device.uboPool->getBuffer< Configuration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
	}

	ShadowMapUbo::~ShadowMapUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void ShadowMapUbo::update( Light const & light
		, uint32_t index )
	{
		CU_Require( m_ubo );
		auto & data = m_ubo.getData();
		data.lightOffset = light.getBufferIndex();
		auto position = light.getParent()->getDerivedPosition();
		data.lightPosFarPlane =
		{
			position[0],
			position[1],
			position[2],
			light.getFarPlane(),
		};

		if ( light.getLightType() == LightType::eDirectional )
		{
			auto directional = light.getDirectionalLight();
			auto & projection = directional->getProjMatrix( index );
			auto & view = directional->getViewMatrix( index );
			data.lightProjection = projection;
			data.lightView = view;
		}
	}

	//*********************************************************************************************
}
