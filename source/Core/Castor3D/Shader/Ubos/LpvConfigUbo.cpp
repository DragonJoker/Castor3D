#include "Castor3D/Shader/Ubos/LpvConfigUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"

#include <CastorUtils/Graphics/Grid.hpp>

namespace castor3d
{
	std::string const LpvConfigUbo::LpvConfig = "LpvConfig";
	std::string const LpvConfigUbo::LightView = "c3d_lightView";
	std::string const LpvConfigUbo::MinVolumeCorner = "c3d_minVolumeCorner";
	std::string const LpvConfigUbo::GridSizes = "c3d_gridSizes";
	std::string const LpvConfigUbo::Config = "c3d_lpvConfig";

	LpvConfigUbo::LpvConfigUbo( RenderDevice const & device
		, uint32_t index )
		: m_device{ device }
		, m_ubo{ m_device.uboPools->getBuffer< Configuration >( 0u ) }
	{
	}
	
	LpvConfigUbo::~LpvConfigUbo()
	{
		m_device.uboPools->putBuffer( m_ubo );
	}

	void LpvConfigUbo::cpuUpdate( Configuration const & config )
	{
		CU_Require( m_ubo );
		auto & data = m_ubo.getData();
		data = config;
	}
}
