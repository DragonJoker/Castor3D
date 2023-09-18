/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereSkyViewPass_H___
#define ___C3DAS_AtmosphereSkyViewPass_H___

#include "AtmosphereScatteringUbo.hpp"

#include <Castor3D/Scene/Background/BackgroundModule.hpp>
#include <Castor3D/Shader/ShaderModule.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

#include <CastorUtils/Design/Named.hpp>

namespace atmosphere_scattering
{
	class AtmosphereSkyViewPass
		: public castor::Named
	{
	public:
		AtmosphereSkyViewPass( crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, castor3d::RenderDevice const & device
			, CameraUbo const & cameraUbo
			, AtmosphereScatteringUbo const & atmosphereUbo
			, crg::ImageViewId const & transmittanceView
			, crg::ImageViewId const & resultView
			, uint32_t index
			, bool const & enabled );
		void accept( castor3d::ConfigurationVisitorBase & visitor );

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
