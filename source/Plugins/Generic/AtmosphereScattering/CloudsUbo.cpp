#include "AtmosphereScattering/CloudsUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

#include <CastorUtils/Graphics/Size.hpp>

#include <ShaderWriter/Source.hpp>

namespace atmosphere_scattering
{
	c3d::MbString const CloudsUbo::Buffer = "C3D_ATM_Clouds";
	c3d::MbString const CloudsUbo::Data = "d";

	CloudsUbo::CloudsUbo( c3d::RenderDevice const & device
		, bool & dirty )
		: UboT{ device }
		, m_config{ dirty }
	{
	}

	void CloudsUbo::cpuUpdate( Configuration const & config
		, float totalTime )
	{
		m_config = config;
		auto & data = getNCData();
		data = config;
		data.crispiness *= 10.0f;
		data.time = totalTime;
		data.windDirection = c3d::point::getNormalised( data.windDirection );
	}
}
