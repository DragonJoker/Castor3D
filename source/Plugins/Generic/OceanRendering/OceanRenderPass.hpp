/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OceanRenderPass_H___
#define ___C3D_OceanRenderPass_H___

#include "OceanUbo.hpp"

#include <Castor3D/Render/Technique/TechniqueModule.hpp>

#include <Castor3D/Render/ShadowMap/ShadowMapDirectional.hpp>
#include <Castor3D/Render/ShadowMap/ShadowMapPoint.hpp>
#include <Castor3D/Render/ShadowMap/ShadowMapSpot.hpp>
#include <Castor3D/Render/Technique/RenderTechniquePass.hpp>

#include <CastorUtils/Miscellaneous/PreciseTimer.hpp>

namespace ocean
{
	class OceanRenderPass
		: public castor3d::RenderTechniquePass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	matrixUbo		The scene matrices UBO.
		 *\param[in]	culler			The culler for this pass.
		 *\param[in]	ssaoConfig		The SSAO configuration.
		 *\param[in]	lpvConfigUbo	The LPV configuration UBO.
		 *\param[in]	llpvConfigUbo	The Layered LPV configuration UBO.
		 *\param[in]	vctConfigUbo	The VCT configuration UBO.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	matrixUbo		L'UBO de matrices de la scène.
		 *\param[in]	culler			Le culler pour cette passe.
		 *\param[in]	ssaoConfig		La configuration du SSAO.
		 *\param[in]	lpvConfigUbo	L'UBO de configuration des LPV.
		 *\param[in]	llpvConfigUbo	L'UBO de configuration des Layered LPV.
		 *\param[in]	vctConfigUbo	L'UBO de configuration du VCT.
		 */
		OceanRenderPass( castor3d::RenderTechnique * parent
			, crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, castor3d::RenderDevice const & device
			, castor::String const & category
			, std::shared_ptr< castor3d::Texture > colourInput
			, std::shared_ptr< castor3d::Texture > depthInput
			, castor3d::SceneRenderPassDesc const & renderPassDesc
			, castor3d::RenderTechniquePassDesc const & techniquePassDesc );
		~OceanRenderPass()override;
		static crg::FramePassArray create( castor3d::RenderDevice const & device
			, castor3d::RenderTechnique & technique
			, castor3d::TechniquePasses & renderPasses
			, crg::FramePassArray previousPass );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		void accept( castor3d::RenderTechniqueVisitor & visitor );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		castor3d::TextureFlags getTexturesMask()const override;

		castor3d::ShaderFlags getShaderFlags()const override
		{
			return castor3d::ShaderFlag::eWorldSpace
				| castor3d::ShaderFlag::eViewSpace
				| castor3d::ShaderFlag::eNormal
				| castor3d::ShaderFlag::eTangentSpace;
		}
		/**@}*/

	public:
		static castor::String const Type;
		static castor::String const Name;
		static castor::String const FullName;
		static castor::String const Param;
		static castor::String const Foam;
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
		void doFillAdditionalDescriptor( castor3d::RenderPipeline const & pipeline
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, castor3d::BillboardRenderNode & node
			, castor3d::ShadowMapLightTypeArray const & shadowMaps )override;
		void doFillAdditionalDescriptor( castor3d::RenderPipeline const & pipeline
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, castor3d::SubmeshRenderNode & node
			, castor3d::ShadowMapLightTypeArray const & shadowMaps )override;
		void doUpdateFlags( castor3d::PipelineFlags & flags )const override;
		void doUpdatePipeline( castor3d::RenderPipeline & pipeline )override;
		castor3d::ShaderPtr doGetVertexShaderSource( castor3d::PipelineFlags const & flags )const override;
		castor3d::ShaderPtr doGetBillboardShaderSource( castor3d::PipelineFlags const & flags )const override;
		castor3d::ShaderPtr doGetHullShaderSource( castor3d::PipelineFlags const & flags )const override;
		castor3d::ShaderPtr doGetDomainShaderSource( castor3d::PipelineFlags const & flags )const override;
		castor3d::ShaderPtr doGetGeometryShaderSource( castor3d::PipelineFlags const & flags )const override;
		castor3d::ShaderPtr doGetPixelShaderSource( castor3d::PipelineFlags const & flags )const override;

	private:
		std::shared_ptr< castor3d::Texture > m_colourInput;
		std::shared_ptr< castor3d::Texture > m_depthInput;
		OceanUboConfiguration m_configuration;
		ashes::SamplerPtr m_linearWrapSampler;
		ashes::SamplerPtr m_pointClampSampler;
		castor::PreciseTimer m_timer;

		OceanUbo m_ubo;
		crg::ImageId m_foamImg;
		crg::ImageViewId m_foamView;
		ashes::ImageView m_foam;
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
