/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SHADOW_MAP_PASS_DIRECTIONAL_H___
#define ___C3D_SHADOW_MAP_PASS_DIRECTIONAL_H___

#include "ShadowMapModule.hpp"

#include "Castor3D/Render/ShadowMap/ShadowMapPass.hpp"
#include "Castor3D/Shader/Ubos/ShadowMapDirectionalUbo.hpp"

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
		 *\param[in]	cascadeCount	The cascades count, used to instantiate objects.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	matrixUbo		L'UBO de matrices de la scène.
		 *\param[in]	culler			Le culler pour cette passe.
		 *\param[in]	shadowMap		La shadow map parente.
		 *\param[in]	cascadeCount	Le nombre de cascades, utilisé pour instancier les objets.
		 */
		C3D_API ShadowMapPassDirectional( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, ShadowMap const & shadowMap
			, uint32_t cascadeCount );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShadowMapPassDirectional()override;
		/**
		 *\copydoc		castor3d::ShadowMapPass::update
		 */
		C3D_API void update( CpuUpdater & updater )override;
		/**
		 *\copydoc		castor3d::ShadowMapPass::update
		 */
		C3D_API void update( GpuUpdater & updater )override;

	private:
		void doUpdateUbos( CpuUpdater & updater )override;
		void doFillAdditionalBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings )const override;
		void doFillAdditionalDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps )override;
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;
		void doUpdateFlags( PipelineFlags & flags )const override;
		ShaderPtr doGetVertexShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPixelShaderSource( PipelineFlags const & flags )const override;

	public:
		C3D_API static uint32_t const TileSize;
		C3D_API static uint32_t const TileCountX;
		C3D_API static uint32_t const TileCountY;
		C3D_API static castor::String const Type;

	private:
		ShadowMapDirectionalUbo m_shadowMapDirectionalUbo;
	};
}

#endif
