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
			, c3d::RenderDevice const & device
			, AtmosphereScatteringUbo const & atmosphereUbo
			, c3d::Texture const & transmittanceLut
			, c3d::Texture & result
			, bool const & enabled );
		void accept( c3d::ConfigurationVisitorBase & visitor );

	private:
		c3d::ShaderModule m_computeShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
	};
}

#endif
