/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereBackgroundPass_H___
#define ___C3DAS_AtmosphereBackgroundPass_H___

#include "AtmosphereScatteringPrerequisites.hpp"

#include <Castor3D/Render/Passes/BackgroundPassBase.hpp>
#include <Castor3D/Scene/Background/Background.hpp>
#include <Castor3D/Shader/ShaderModule.hpp>

#include <CastorUtils/Design/DataHolder.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

namespace atmosphere_scattering
{
	struct Shaders
	{
		c3d::ProgramModule shader;
		ashes::PipelineShaderStageCreateInfoArray stages;
	};

	enum class AtmosphereBackgroundBindings : uint32_t
	{
		eScene,
		eRenderConfig,
		eClouds,
		eCount,
	};

	class AtmosphereBackgroundPass
		: public c3d::DataHolderT< Shaders >
		, public c3d::BackgroundPassBase
		, public crg::RenderQuad
	{
	public:
	public:
		AtmosphereBackgroundPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, c3d::RenderDevice const & device
			, AtmosphereBackground & background
			, c3d::Extent2D const & size
			, bool forceVisible );

	private:
		void doResetPipeline( uint32_t index )override;

		crg::VkPipelineShaderStageCreateInfoArray doInitialiseShader( c3d::RenderDevice const & device
			, c3d::Extent2D const & size
			, uint32_t passIndex );
	};
}

#endif
