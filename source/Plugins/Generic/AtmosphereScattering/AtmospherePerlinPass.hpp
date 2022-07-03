/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmospherePerlinPass_H___
#define ___C3DAS_AtmospherePerlinPass_H___

#include "AtmosphereScatteringPrerequisites.hpp"

#include <Castor3D/Miscellaneous/MiscellaneousModule.hpp>
#include <Castor3D/Shader/ShaderModule.hpp>

#include <ashespp/Pipeline/PipelineShaderStageCreateInfo.hpp>

namespace atmosphere_scattering
{
	class AtmospherePerlinPass
	{
	public:
		AtmospherePerlinPass( crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, castor3d::RenderDevice const & device
			, crg::ImageViewId const & resultView
			, bool & enabled );
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
