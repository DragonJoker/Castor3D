/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_CloudsWeatherPass_H___
#define ___C3DAS_CloudsWeatherPass_H___

#include "WeatherUbo.hpp"

#include <Castor3D/Scene/Background/BackgroundModule.hpp>
#include <Castor3D/Shader/ShaderModule.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

namespace atmosphere_scattering
{
	class CloudsWeatherPass
	{
	public:
		CloudsWeatherPass( crg::FramePassGroup & graph
			, c3d::RenderDevice const & device
			, WeatherUbo const & weatherUbo
			, c3d::Texture & result
			, bool const & enabled );
		void accept( c3d::ConfigurationVisitorBase & visitor )const;

	private:
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
	};
}

#endif
