/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_CloudsResolvePass_H___
#define ___C3DAS_CloudsResolvePass_H___

#include "CloudsUbo.hpp"

#include <Castor3D/Scene/Background/BackgroundModule.hpp>
#include <Castor3D/Shader/ShaderModule.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

#include <CastorUtils/Design/Named.hpp>

namespace atmosphere_scattering
{
	class CloudsResolvePass
		: public c3d::Named
	{
	public:
		CloudsResolvePass( crg::FramePassGroup & graph
			, c3d::RenderDevice const & device
			, CameraUbo const & cameraUbo
			, AtmosphereScatteringUbo const & atmosphereUbo
			, CloudsUbo const & cloudsUbo
			, c3d::Texture const & transmittance
			, c3d::Texture const & multiscatter
			, c3d::Texture const & skyview
			, c3d::Texture const & volume
			, c3d::Texture const & inscatterResult
			, c3d::Texture const & transmittanceResult
			, c3d::Texture const * depthObj
			, c3d::Texture & result
			, uint32_t index );
		void accept( c3d::ConfigurationVisitorBase & visitor )const;

	private:
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
	};
}

#endif
