/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredTransparentPass_H___
#define ___C3D_DeferredTransparentPass_H___

#include "TransparentModule.hpp"

#include "Castor3D/Render/ShadowMap/ShadowMapDirectional.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPoint.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapSpot.hpp"
#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"

namespace castor3d
{
	class TransparentPass
		: public RenderTechniquePass
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
		C3D_API TransparentPass( RenderDevice const & device
			, castor::Size const & size
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, SsaoConfig const & ssaoConfig
			, TransparentPassResult const & transparentPassResult
			, LpvGridConfigUbo const & lpvConfigUbo
			, LayeredLpvGridConfigUbo const & llpvConfigUbo
			, VoxelizerUbo const & vctConfigUbo
			, LightVolumePassResult const & lpvResult
			, TextureUnit const & vctFirstBounce
			, TextureUnit const & vctSecondaryBounce );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~TransparentPass();
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor )override;
		/**
		 *\~english
		 *\brief		Renders transparent nodes.
		 *\param[in]	device	The GPU device.
		 *\param[in]	toWait	The semaphore to wait for.
		 *\~french
		 *\brief		Dessine les noeuds transparents.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	toWait	Le sémaphore à attendre.
		 */
		C3D_API ashes::Semaphore const & render( ashes::Semaphore const & toWait );

		C3D_API TextureFlags getTexturesMask()const override;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		void setDepthFormat( VkFormat value )
		{
			m_depthFormat = value;
		}
		/**@}*/

	private:
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;
		ShaderPtr doGetPhongPixelShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const override;
		void doUpdatePipeline( RenderPipeline & pipeline )override;

	private:
		VkFormat m_depthFormat{};
		ashes::CommandBufferPtr m_nodesCommands;
		ashes::FrameBufferPtr m_frameBuffer;
	};
}

#endif
