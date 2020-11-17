#include "Castor3D/Shader/Ubos/LpvLightConfigUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"

#include <CastorUtils/Graphics/Grid.hpp>

CU_ImplementCUSmartPtr( castor3d, LpvLightConfigUbo )

namespace castor3d
{
	std::string const LpvLightConfigUbo::LpvLightConfig = "LpvLightConfig";
	std::string const LpvLightConfigUbo::LightView = "c3d_lightView";
	std::string const LpvLightConfigUbo::Config1 = "c3d_lpvConfig1";
	std::string const LpvLightConfigUbo::Config2 = "c3d_lpvConfig2";

	LpvLightConfigUbo::LpvLightConfigUbo()
	{
	}

	LpvLightConfigUbo::LpvLightConfigUbo( RenderDevice const & device )
	{
		initialise( device );
	}
	
	LpvLightConfigUbo::~LpvLightConfigUbo()
	{
		cleanup();
	}

	void LpvLightConfigUbo::initialise( RenderDevice const & device )
	{
		m_device = &device;
		m_ubo = m_device->uboPools->getBuffer< Configuration >( 0u );
	}

	void LpvLightConfigUbo::cleanup()
	{
		if ( m_ubo )
		{
			m_device->uboPools->putBuffer( m_ubo );
			m_device = nullptr;
			m_ubo = {};
		}
	}

	void LpvLightConfigUbo::cpuUpdate( Light const & light )
	{
		CU_Require( m_ubo );
		auto & lpvConfig = light.getLpvConfig();
		auto & configuration = m_ubo.getData();

		configuration.lightIndex = float( light.getBufferIndex() );
		configuration.indirectAttenuation = lpvConfig.indirectAttenuation;
		configuration.texelAreaModifier = lpvConfig.texelAreaModifier;
		auto ltType = light.getLightType();

		switch ( ltType )
		{
		case LightType::eDirectional:
			configuration.lightView = light.getDirectionalLight()->getViewMatrix( 3u );
			configuration.tanFovXHalf = 1.0f;
			configuration.tanFovYHalf = 1.0f;
			break;

		case LightType::ePoint:
			break;

		case LightType::eSpot:
			{
				auto & spotLight = *light.getSpotLight();
				configuration.lightView = spotLight.getViewMatrix();
				auto lightFov = spotLight.getCutOff();
				configuration.tanFovXHalf = ( lightFov * 0.5 ).tan();
				configuration.tanFovYHalf = ( lightFov * 0.5 ).tan();
			}
			break;
		}
	}

	void LpvLightConfigUbo::cpuUpdate( DirectionalLight const & light
		, uint32_t cascadeIndex )
	{
		auto & lpvConfig = light.getLight().getLpvConfig();
		auto & configuration = m_ubo.getData();

		configuration.indirectAttenuation = lpvConfig.indirectAttenuation;
		configuration.texelAreaModifier = lpvConfig.texelAreaModifier;
		configuration.lightIndex = float( light.getLight().getBufferIndex() );
		configuration.lightView = light.getViewMatrix( cascadeIndex );
		configuration.tanFovXHalf = 1.0f;
		configuration.tanFovYHalf = 1.0f;
	}
}
