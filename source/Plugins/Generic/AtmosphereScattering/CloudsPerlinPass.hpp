/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_CloudsPerlinPass_H___
#define ___C3DAS_CloudsPerlinPass_H___

#include "AtmosphereScatteringPrerequisites.hpp"

#include <Castor3D/Miscellaneous/MiscellaneousModule.hpp>
#include <Castor3D/Shader/ShaderModule.hpp>

#include <ashespp/Pipeline/PipelineShaderStageCreateInfo.hpp>

namespace atmosphere_scattering
{
	class CloudsPerlinPass
	{
	public:
		CloudsPerlinPass( crg::FramePassGroup & graph
			, c3d::RenderDevice const & device
			, c3d::Texture & result
			, bool & enabled );
		void accept( c3d::ConfigurationVisitorBase & visitor )const;

	private:
		c3d::ShaderModule m_computeShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
	};
}

#endif
