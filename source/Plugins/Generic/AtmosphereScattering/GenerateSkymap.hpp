/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_GenerateSkymapPass_H___
#define ___C3DAS_GenerateSkymapPass_H___

#include "AtmosphereScatteringPrerequisites.hpp"

#include <Castor3D/Render/RenderModule.hpp>
#include <Castor3D/Render/Technique/TechniqueModule.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ashespp/Pipeline/PipelineShaderStageCreateInfo.hpp>

namespace atmosphere_scattering
{
	crg::FramePass const * createGenerateSkymapPass( castor3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, crg::FramePassArray previousPasses
		, AtmosphereScatteringUbo const & oceanUbo
		, crg::ImageViewId const & transmittance
		, crg::ImageViewId const & inscatter
		, crg::ImageViewId const & skyIrradiance
		, crg::ImageViewId const & colourOutput
		, crg::RunnablePass::IsEnabledCallback isEnabled
		, ashes::PipelineShaderStageCreateInfoArray & modules );
}

#endif
