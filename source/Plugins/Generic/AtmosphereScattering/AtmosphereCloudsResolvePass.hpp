/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereCloudsResolvePass_H___
#define ___C3DAS_AtmosphereCloudsResolvePass_H___

#include "AtmosphereWeatherUbo.hpp"

#include <Castor3D/Scene/Background/BackgroundModule.hpp>
#include <Castor3D/Shader/ShaderModule.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

#include <CastorUtils/Design/Named.hpp>

namespace atmosphere_scattering
{
	class AtmosphereCloudsResolvePass
		: public castor::Named
	{
	public:
		AtmosphereCloudsResolvePass( crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, castor3d::RenderDevice const & device
			, CameraUbo const & cameraUbo
			, AtmosphereScatteringUbo const & atmosphereUbo
			, crg::ImageViewId const & clouds
			, crg::ImageViewId const & emission
			, crg::ImageViewId const & resultView
			, uint32_t index );
		void accept( castor3d::PipelineVisitor & visitor );

		crg::FramePass const & getLastPass()const
		{
			return *m_lastPass;
		}

	private:
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::FramePass const * m_lastPass;
	};
}

#endif
