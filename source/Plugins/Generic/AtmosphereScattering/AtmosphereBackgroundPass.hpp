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
		castor3d::ShaderModule vertexShader;
		castor3d::ShaderModule pixelShader;
		ashes::PipelineShaderStageCreateInfoArray stages;
	};

	class AtmosphereBackgroundPass
		: public castor::DataHolderT< Shaders >
		, public castor3d::BackgroundPassBase
		, public crg::RenderQuad
	{
	public:
		enum Bindings : uint32_t
		{
			eScene,
			eHdrConfig,
			eClouds,
			eCount,
		};

	public:
		AtmosphereBackgroundPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, castor3d::RenderDevice const & device
			, AtmosphereBackground & background
			, VkExtent2D const & size
			, crg::ImageViewIdArray const & colour
			, bool forceVisible );

	private:
		void doResetPipeline( uint32_t index )override;

		crg::VkPipelineShaderStageCreateInfoArray doInitialiseShader( castor3d::RenderDevice const & device
			, AtmosphereBackground & background
			, VkExtent2D const & size
			, uint32_t passIndex );
	};
}

#endif
