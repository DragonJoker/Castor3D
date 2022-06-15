/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereVolumePass_H___
#define ___C3DAS_AtmosphereVolumePass_H___

#include "AtmosphereScatteringUbo.hpp"

#include <Castor3D/Scene/Background/BackgroundModule.hpp>
#include <Castor3D/Shader/ShaderModule.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

namespace atmosphere_scattering
{
	class AtmosphereVolumePass
	{
	public:
		AtmosphereVolumePass( crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, castor3d::RenderDevice const & device
			, castor3d::MatrixUbo const & matrixUbo
			, castor3d::SceneUbo const & sceneUbo
			, AtmosphereScatteringUbo const & atmosphereUbo
			, crg::ImageViewId const & transmittanceView
			, crg::ImageViewId const & multiScatterView
			, crg::ImageViewId const & resultView );
		void accept( castor3d::BackgroundVisitor & visitor );

		crg::FramePass const & getLastPass()const
		{
			return *m_lastPass;
		}

	private:
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_geometryShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::FramePass const * m_lastPass;
	};
}

#endif
