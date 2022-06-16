/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereTransmittancePass_H___
#define ___C3DAS_AtmosphereTransmittancePass_H___

#include "AtmosphereScatteringUbo.hpp"

#include <Castor3D/Scene/Background/BackgroundModule.hpp>
#include <Castor3D/Shader/ShaderModule.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

namespace atmosphere_scattering
{
	class AtmosphereTransmittancePass
	{
	public:
		AtmosphereTransmittancePass( crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, castor3d::RenderDevice const & device
			, AtmosphereScatteringUbo const & atmosphereUbo
			, crg::ImageViewId const & resultView );
		void accept( castor3d::BackgroundVisitor & visitor );

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
