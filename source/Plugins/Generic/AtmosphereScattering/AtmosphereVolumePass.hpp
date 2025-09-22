/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereVolumePass_H___
#define ___C3DAS_AtmosphereVolumePass_H___

#include "AtmosphereScatteringUbo.hpp"

#include <Castor3D/Scene/Background/BackgroundModule.hpp>
#include <Castor3D/Shader/ShaderModule.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

#include <CastorUtils/Design/Named.hpp>

namespace atmosphere_scattering
{
	class AtmosphereVolumePass
		: public c3d::Named
		
	{
	public:
		AtmosphereVolumePass( crg::FramePassGroup & graph
			, c3d::RenderDevice const & device
			, CameraUbo const & cameraUbo
			, AtmosphereScatteringUbo const & atmosphereUbo
			, c3d::Texture const & transmittance
			, c3d::Texture & result
			, uint32_t index
			, bool const & enabled );
		void accept( c3d::ConfigurationVisitorBase & visitor )const;

	private:
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
	};
}

#endif
