/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_RenderSkyPass_H___
#define ___C3DAS_RenderSkyPass_H___

#include "AtmosphereScatteringPrerequisites.hpp"

#include <Castor3D/Render/RenderModule.hpp>
#include <Castor3D/Render/Technique/TechniqueModule.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ashespp/Pipeline/PipelineShaderStageCreateInfo.hpp>

namespace atmosphere_scattering
{
	crg::FramePass const * createRenderSkyPass( castor3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, crg::FramePassArray previousPasses
		, AtmosphereScatteringUbo const & atmosphereUbo
		, castor3d::MatrixUbo const & matrixUbo
		, castor3d::SceneUbo const & sceneUbo
		, crg::ImageViewId const & transmittance
		, crg::ImageViewId const & inscatter
		, crg::ImageViewId const & colourOutput
		, crg::RunnablePass::IsEnabledCallback isEnabled
		, ashes::PipelineShaderStageCreateInfoArray & modules );
}

#endif
