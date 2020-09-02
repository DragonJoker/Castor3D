#include "Castor3D/Shader/Ubos/ShadowMapUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

namespace castor3d
{
	uint32_t const ShadowMapUbo::BindingPoint = 13u;
	castor::String const ShadowMapUbo::BufferShadowMap = cuT( "ShadowMapCfg" );
	castor::String const ShadowMapUbo::LightProjection = cuT( "c3d_lightProjection" );
	castor::String const ShadowMapUbo::LightView = cuT( "c3d_lightView" );
	castor::String const ShadowMapUbo::LightPosFarPlane = cuT( "c3d_lightPosFarPlane" );
	castor::String const ShadowMapUbo::LightIndex = cuT( "c3d_lightIndex" );

	ShadowMapUbo::ShadowMapUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentRenderDevice() )
		{
			initialise();
		}
	}

	ShadowMapUbo::~ShadowMapUbo()
	{
	}

	void ShadowMapUbo::initialise()
	{
		if ( !m_ubo )
		{
			m_ubo = m_engine.getUboPools().getBuffer< Configuration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		}
	}

	void ShadowMapUbo::cleanup()
	{
		if ( m_ubo )
		{
			m_engine.getUboPools().putBuffer( m_ubo );
		}
	}

	void ShadowMapUbo::update( Light const & light
		, uint32_t index )
	{
		CU_Require( m_ubo );
		auto & data = m_ubo.getData();
		data.lightIndex = light.getBufferIndex();
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
}
