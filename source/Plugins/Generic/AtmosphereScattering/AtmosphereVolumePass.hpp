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
			, crg::FramePassArray const & previousPasses
			, c3d::RenderDevice const & device
			, CameraUbo const & cameraUbo
			, AtmosphereScatteringUbo const & atmosphereUbo
			, crg::ImageViewId const & transmittanceView
			, crg::ImageViewId const & resultView
			, uint32_t index
			, bool const & enabled );
		void accept( c3d::ConfigurationVisitorBase & visitor );

		crg::FramePass const & getLastPass()const
		{
			return *m_lastPass;
		}

	private:
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::FramePass const * m_lastPass;
	};
}

#endif
