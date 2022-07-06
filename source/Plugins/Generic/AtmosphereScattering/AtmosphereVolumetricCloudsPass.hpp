/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereVolumetricCloudsPass_H___
#define ___C3DAS_AtmosphereVolumetricCloudsPass_H___

#include "CloudsUbo.hpp"

#include <Castor3D/Scene/Background/BackgroundModule.hpp>
#include <Castor3D/Shader/ShaderModule.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

#include <CastorUtils/Design/Named.hpp>

namespace atmosphere_scattering
{
	class AtmosphereVolumetricCloudsPass
		: public castor::Named
	{
	public:
		AtmosphereVolumetricCloudsPass( crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, castor3d::RenderDevice const & device
			, AtmosphereScatteringUbo const & atmosphereUbo
			, CameraUbo const & cameraUbo
			, CloudsUbo const & weatherUbo
			, crg::ImageViewId const & transmittance
			, crg::ImageViewId const & multiscatter
			, crg::ImageViewId const & skyview
			, crg::ImageViewId const & volume
			, crg::ImageViewId const & perlinWorley
			, crg::ImageViewId const & worley
			, crg::ImageViewId const & curl
			, crg::ImageViewId const & weather
			, crg::ImageViewId const & colourResult
			, crg::ImageViewId const & emissionResult
			, uint32_t index );
		void accept( castor3d::PipelineVisitor & visitor );

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
