/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereMultiScatteringPass_H___
#define ___C3DAS_AtmosphereMultiScatteringPass_H___

#include "AtmosphereScatteringUbo.hpp"

#include <Castor3D/Scene/Background/BackgroundModule.hpp>
#include <Castor3D/Shader/ShaderModule.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

namespace atmosphere_scattering
{
	class AtmosphereMultiScatteringPass
	{
	public:
		AtmosphereMultiScatteringPass( crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, castor3d::RenderDevice const & device
			, AtmosphereScatteringUbo const & atmosphereUbo
			, crg::ImageViewId const & transmittanceLut
			, crg::ImageViewId const & resultView );
		void accept( castor3d::PipelineVisitor & visitor );

		crg::FramePass const & getLastPass()const
		{
			return *m_lastPass;
		}

	private:
		castor3d::ShaderModule m_computeShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::FramePass const * m_lastPass;
	};
}

#endif
