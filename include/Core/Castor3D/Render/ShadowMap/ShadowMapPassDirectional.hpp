/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SHADOW_MAP_PASS_DIRECTIONAL_H___
#define ___C3D_SHADOW_MAP_PASS_DIRECTIONAL_H___

#include "ShadowMapModule.hpp"

#include "Castor3D/Render/ShadowMap/ShadowMapPass.hpp"

namespace castor3d
{
	class ShadowMapPassDirectional
		: public ShadowMapPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	matrixUbo		The scene matrices UBO.
		 *\param[in]	culler			The culler for this pass.
		 *\param[in]	shadowMap		The parent shadow map.
		 *\param[in]	cascadeIndex	The index of the cascade of this pass.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	matrixUbo		L'UBO de matrices de la scène.
		 *\param[in]	culler			Le culler pour cette passe.
		 *\param[in]	shadowMap		La shadow map parente.
		 *\param[in]	cascadeIndex	L'index de la cascade de cette passe.
		 */
		C3D_API ShadowMapPassDirectional( RenderDevice const & device
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, ShadowMap const & shadowMap
			, uint32_t cascadeIndex );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShadowMapPassDirectional();
		/**
		 *\copydoc		castor3d::ShadowMapPass::update
		 */
		C3D_API bool update( CpuUpdater & updater )override;
		/**
		 *\copydoc		castor3d::ShadowMapPass::update
		 */
		C3D_API void update( GpuUpdater & updater )override;

	private:
		void doUpdateUbos( CpuUpdater & updater )override;
		void doFillUboDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
			, BillboardListRenderNode & node )override;
		void doFillUboDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
			, SubmeshRenderNode & node )override;
		ashes::VkDescriptorSetLayoutBindingArray doCreateUboBindings( PipelineFlags const & flags )const override;
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;
		void doUpdateFlags( PipelineFlags & flags )const override;
		ShaderPtr doGetVertexShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPhongPixelShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const override;

	public:
		C3D_API static uint32_t const TextureSize;
	};
}

#endif
