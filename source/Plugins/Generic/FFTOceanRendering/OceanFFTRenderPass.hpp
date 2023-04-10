/*
See LICENSE file in root folder
*/
#ifndef ___C3DORFFT_OceanRenderPass_H___
#define ___C3DORFFT_OceanRenderPass_H___

#include "OceanFFTUbo.hpp"
#include "OceanFFT.hpp"

#include <Castor3D/Render/RenderModule.hpp>

#include <Castor3D/Render/RenderTechniquePass.hpp>
#include <Castor3D/Render/ShadowMap/ShadowMapDirectional.hpp>
#include <Castor3D/Render/ShadowMap/ShadowMapPoint.hpp>
#include <Castor3D/Render/ShadowMap/ShadowMapSpot.hpp>

#include <CastorUtils/Miscellaneous/PreciseTimer.hpp>

namespace ocean_fft
{
	class OceanRenderPass
		: public castor3d::RenderTechniqueNodesPass
	{
	public:
		OceanRenderPass( castor3d::RenderTechnique * parent
			, crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, castor3d::RenderDevice const & device
			, std::unique_ptr< OceanUbo > oceanUbo
			, std::unique_ptr< OceanFFT > oceanFFT
			, crg::ImageViewIdArray targetImage
			, crg::ImageViewIdArray targetDepth
			, castor3d::RenderNodesPassDesc const & renderPassDesc
			, castor3d::RenderTechniquePassDesc const & techniquePassDesc
			, castor3d::IsRenderPassEnabledUPtr isEnabled );
		~OceanRenderPass()override;
		static crg::FramePassArray create( castor3d::RenderDevice const & device
			, castor3d::RenderTechnique & technique
			, castor3d::TechniquePasses & renderPasses
			, crg::FramePassArray previousPass );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		void accept( castor3d::RenderTechniqueVisitor & visitor )override;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		castor3d::ComponentModeFlags getComponentsMask()const override;
		castor3d::ShaderFlags getShaderFlags()const override;
		/**@}*/

	public:
		static castor::String const Type;
		static castor::String const FullName;
		static castor::String const Param;
		static castor::String const ParamFFT;
		static castor3d::TechniquePassEvent constexpr Event{ castor3d::TechniquePassEvent::eBeforeTransparent };

	private:
		void doUpdateUbos( castor3d::CpuUpdater & updater )override;
		bool doIsValidPass( castor3d::Pass const & pass )const override;

		void doFillAdditionalBindings( castor3d::PipelineFlags const & flags
			, ashes::VkDescriptorSetLayoutBindingArray & bindings )const override;
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( castor3d::PipelineFlags const & flags )const override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( castor3d::PipelineFlags const & flags )const override;
		void doFillAdditionalDescriptor( castor3d::PipelineFlags const & flags
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, castor3d::ShadowMapLightTypeArray const & shadowMaps )override;
		castor3d::SubmeshFlags doAdjustSubmeshFlags( castor3d::SubmeshFlags flags )const override;
		castor3d::ProgramFlags doAdjustProgramFlags( castor3d::ProgramFlags flags )const override;
		void doAdjustFlags( castor3d::PipelineFlags & flags )const override;
		castor3d::ShaderPtr doGetVertexShaderSource( castor3d::PipelineFlags const & flags )const override;
		castor3d::ShaderPtr doGetBillboardShaderSource( castor3d::PipelineFlags const & flags )const override;
		castor3d::ShaderPtr doGetHullShaderSource( castor3d::PipelineFlags const & flags )const override;
		castor3d::ShaderPtr doGetDomainShaderSource( castor3d::PipelineFlags const & flags )const override;
		castor3d::ShaderPtr doGetPixelShaderSource( castor3d::PipelineFlags const & flags )const override;

	private:
		castor3d::IsRenderPassEnabledUPtr m_isEnabled;
		std::unique_ptr< OceanUbo > m_ubo;
		std::unique_ptr< OceanFFT > m_oceanFFT;
		OceanUboConfiguration m_configuration;
		ashes::SamplerPtr m_linearWrapSampler;
		ashes::SamplerPtr m_pointClampSampler;
		castor::PreciseTimer m_timer;
	};
}

#endif
