/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_CloudsVolumePass_H___
#define ___C3DAS_CloudsVolumePass_H___

#include "CloudsUbo.hpp"

#include <Castor3D/Scene/Background/BackgroundModule.hpp>
#include <Castor3D/Shader/ShaderModule.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

#include <CastorUtils/Design/Named.hpp>

namespace atmosphere_scattering
{
	class CloudsVolumePass
		: public c3d::Named
	{
	public:
		CloudsVolumePass( crg::FramePassGroup & graph
			, c3d::RenderDevice const & device
			, c3d::CameraUbo const & mainCameraUbo
			, c3d::Texture const & transmittance
			, c3d::Camera const & camera
			, c3d::Texture const * depthObj
			, c3d::Texture & scatteringResult
			, c3d::Texture & transmittanceResult
			, uint32_t index );
		void accept( c3d::ConfigurationVisitorBase & visitor )const;

	private:
		c3d::Vector< crg::AttachmentPtr > m_attachs;
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
	};
}

#endif
