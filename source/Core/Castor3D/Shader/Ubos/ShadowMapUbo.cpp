#include "Castor3D/Shader/Ubos/ShadowMapUbo.hpp"

#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

#include <ShaderWriter/Source.hpp>

namespace c3d
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
			return 1.0f - clamp( length( pos - lightPosFarPlane().xyz() ) / lightPosFarPlane().w(), 0.0_f, 1.0_f );
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

	ShadowMapUbo::~ShadowMapUbo()noexcept
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void ShadowMapUbo::update( LightInstance const & light
		, uint32_t index )
	{
		CU_Require( m_ubo );
		auto & data = m_ubo.getData();
		data.lightOffset = light.getBufferIndex();
		auto position = light.getNode().getDerivedPosition();
		data.lightPosFarPlane =
		{
			position[0],
			position[1],
			position[2],
			light.getCategory().getFarPlane(),
		};

		if ( light.getCategory().getLightType() == LightType::eDirectional )
		{
			auto & directional = static_cast< DirectionalLightInstance const & >( light );
			auto & projection = directional.getProjMatrix( index );
			auto & view = directional.getViewMatrix( index );
			data.lightProjection = projection;
			data.lightView = view;
		}
	}

	//*********************************************************************************************
}
