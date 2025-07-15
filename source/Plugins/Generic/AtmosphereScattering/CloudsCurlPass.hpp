/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_CloudsCurlPass_H___
#define ___C3DAS_CloudsCurlPass_H___

#include "CloudsUbo.hpp"

#include <Castor3D/Scene/Background/BackgroundModule.hpp>
#include <Castor3D/Shader/ShaderModule.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

namespace atmosphere_scattering
{
	class CloudsCurlPass
	{
	public:
		CloudsCurlPass( crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, c3d::RenderDevice const & device
			, crg::ImageViewId const & resultView
			, bool & enabled );
		void accept( c3d::ConfigurationVisitorBase & visitor );

		crg::FramePass const & getLastPass()const
		{
			return *m_lastPass;
		}

	private:
		c3d::ShaderModule m_computeShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::FramePass const * m_lastPass;
	};
}

#endif
