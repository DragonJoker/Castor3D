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
			, crg::FramePassArray const & previousPasses
			, castor3d::RenderDevice const & device
			, WeatherUbo const & weatherUbo
			, crg::ImageViewId const & resultView
			, bool const & enabled );
		void accept( castor3d::ConfigurationVisitorBase & visitor );

		crg::FramePass const & getLastPass()const
		{
			return *m_lastPass;
		}

	private:
		castor3d::ShaderModule m_computeShader;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_fragmentShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::FramePass const * m_lastPass;
	};
}

#endif
