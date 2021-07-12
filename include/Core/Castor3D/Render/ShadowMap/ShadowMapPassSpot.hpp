/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SHADOW_MAP_PASS_SPOT_H___
#define ___C3D_SHADOW_MAP_PASS_SPOT_H___

#include "ShadowMapModule.hpp"

#include "Castor3D/Render/ShadowMap/ShadowMapPass.hpp"

namespace castor3d
{
	class ShadowMapPassSpot
		: public ShadowMapPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	index		The pass index.
		 *\param[in]	matrixUbo	The scene matrices UBO.
		 *\param[in]	culler		The culler for this pass.
		 *\param[in]	shadowMap	The parent shadow map.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	index		L'indice de la passe.
		 *\param[in]	matrixUbo	L'UBO de matrices de la scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 *\param[in]	shadowMap	La shadow map parente.
		 */
		C3D_API ShadowMapPassSpot( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, uint32_t index
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, ShadowMap const & shadowMap );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShadowMapPassSpot();
		/**
		 *\copydoc		castor3d::ShadowMapPass::update
		 */
		bool update( CpuUpdater & updater )override;
		/**
		 *\copydoc		castor3d::ShadowMapPass::update
		 */
		void update( GpuUpdater & updater )override;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		ShadowType getShadowType()const
		{
			return m_shadowType;
		}
		/**@}*/

	private:
		void doUpdateUbos( CpuUpdater & updater )override;
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;
		void doUpdateFlags( PipelineFlags & flags )const override;
		ShaderPtr doGetVertexShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPhongPixelShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPbrPixelShaderSource( PipelineFlags const & flags )const override;

	public:
		C3D_API static uint32_t const TextureSize;

	private:
		ShadowType m_shadowType{ ShadowType::eRaw };
		castor::Matrix4x4f m_view;
	};
}

#endif
