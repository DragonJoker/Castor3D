/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VisibilityReorderPass_H___
#define ___C3D_VisibilityReorderPass_H___

#include "OpaqueModule.hpp"

#include "Castor3D/Shader/ShaderModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <CastorUtils/Design/Named.hpp>

#include <RenderGraph/RunnablePass.hpp>

#include <ashespp/Pipeline/PipelineShaderStageCreateInfo.hpp>

namespace c3d
{
	class VisibilityReorderPass
		: public Named
	{
	public:
		VisibilityReorderPass( crg::FramePassGroup & graph
			, RenderDevice const & device
			, Texture const & data
			, Buffer & materialsCounts
			, Buffer & indirectCounts
			, Buffer & materialsStarts
			, Buffer & pixels
			, crg::RunnablePass::IsEnabledCallback isEnabled );
		void accept( ConfigurationVisitorBase & visitor )const;

	private:
		ShaderModule m_computeCountsShader;
		ashes::PipelineShaderStageCreateInfoArray m_countsStages;
		ShaderModule m_computeStartsShader;
		ashes::PipelineShaderStageCreateInfoArray m_startsStages;
		ShaderModule m_computePixelsShader;
		ashes::PipelineShaderStageCreateInfoArray m_pixelsStages;
	};
}

#endif
