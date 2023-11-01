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
		: public castor::Named
	{
	public:
		CloudsResolvePass( crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, castor3d::RenderDevice const & device
			, CameraUbo const & cameraUbo
			, AtmosphereScatteringUbo const & atmosphereUbo
			, CloudsUbo const & cloudsUbo
			, crg::ImageViewId const & sky
			, crg::ImageViewId const & sun
			, crg::ImageViewId const & clouds
			, crg::ImageViewId const & resultView
			, uint32_t index );
		void accept( castor3d::ConfigurationVisitorBase & visitor );

		crg::FramePass const & getLastPass()const
		{
			return *m_lastPass;
		}

	private:
		castor3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::FramePass const * m_lastPass;
	};
}

#endif
