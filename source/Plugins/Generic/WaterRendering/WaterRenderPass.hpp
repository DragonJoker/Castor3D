/*
See LICENSE file in root folder
*/
#ifndef ___C3D_WaterRenderPass_H___
#define ___C3D_WaterRenderPass_H___

#include "WaterUbo.hpp"

#include <Castor3D/Render/RenderModule.hpp>

#include <Castor3D/Render/RenderTechniquePass.hpp>
#include <Castor3D/Render/ShadowMap/ShadowMapDirectional.hpp>
#include <Castor3D/Render/ShadowMap/ShadowMapPoint.hpp>
#include <Castor3D/Render/ShadowMap/ShadowMapSpot.hpp>

#include <CastorUtils/Miscellaneous/PreciseTimer.hpp>

namespace water
{
	class WaterRenderPass
		: public castor3d::RenderTechniqueNodesPass
	{
	public:
		WaterRenderPass( castor3d::RenderTechnique * parent
			, crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, castor3d::RenderDevice const & device
			, crg::ImageViewIdArray targetImage
			, crg::ImageViewIdArray targetDepth
			, castor3d::RenderNodesPassDesc const & renderPassDesc
			, castor3d::RenderTechniquePassDesc const & techniquePassDesc
			, castor3d::IsRenderPassEnabledUPtr isEnabled );
		~WaterRenderPass()override;
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
		castor3d::ShaderFlags getShaderFlags()const override;
		/**@}*/

	public:
		static castor::String const Type;
		static castor::String const Name;
		static castor::String const FullName;
		static castor::String const Param;
		static castor::String const Normals1;
		static castor::String const Normals2;
		static castor::String const Noise;
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
			, castor3d::ShadowMapLightTypeArray const & shadowMaps
			, castor3d::ShadowBuffer const * shadowBuffer )override;
		castor3d::SubmeshComponentCombine doAdjustSubmeshComponents( castor3d::SubmeshComponentCombine submeshCombine )const override;
		castor3d::ProgramFlags doAdjustProgramFlags( castor3d::ProgramFlags flags )const override;
		void doGetPixelShaderSource( castor3d::PipelineFlags const & flags
			, ast::ShaderBuilder & builder )const override;

	private:
		castor3d::IsRenderPassEnabledUPtr m_isEnabled;
		WaterUboConfiguration m_configuration;
		ashes::SamplerPtr m_linearWrapSampler;
		ashes::SamplerPtr m_pointClampSampler;
		castor::PreciseTimer m_timer;

		WaterUbo m_ubo;
		crg::ImageId m_normals1Img;
		crg::ImageViewId m_normals1View;
		ashes::ImageView m_normals1;
		crg::ImageId m_normals2Img;
		crg::ImageViewId m_normals2View;
		ashes::ImageView m_normals2;
		crg::ImageId m_noiseImg;
		crg::ImageViewId m_noiseView;
		ashes::ImageView m_noise;
	};
}

#endif
