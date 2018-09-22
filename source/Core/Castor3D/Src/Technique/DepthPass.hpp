/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DepthPass_H___
#define ___C3D_DepthPass_H___

#include "RenderTechniquePass.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Deferred lighting Render technique pass.
	\~french
	\brief		Classe de passe de technique de rendu implémentant le Deferred lighting.
	*/
	class DepthPass
		: public RenderTechniquePass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor for opaque nodes.
		 *\param[in]	name		This pass name.
		 *\param[in]	scene		The rendered scene.
		 *\param[in]	camera		The camera for this pass.
		 *\param[in]	ssaoConfig	The SSAO configuration.
		 *\param[in]	depthBuffer	The target depth buffer.
		 *\~french
		 *\brief		Constructeur pour les noeuds opaques.
		 *\param[in]	name		Le nom de cette passe.
		 *\param[in]	scene		La scène rendue.
		 *\param[in]	camera		La caméra pour cette passe.
		 *\param[in]	ssaoConfig	La configuration du SSAO.
		 *\param[in]	depthBuffer	Le tampon de profondeur cible.
		 */
		DepthPass( castor::String const & name
			, MatrixUbo const & matrixUbo
			, SceneCuller & culler
			, SsaoConfig const & ssaoConfig
			, TextureLayoutSPtr depthBuffer );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~DepthPass();
		/**
		 *\copydoc		castor3d::RenderTechniquePass::render
		 */
		void update( RenderInfo & info
			, castor::Point2r const & jitter )override;

	private:
		/**
		 *\copydoc		castor3d::RenderPass::doUpdate
		 */
		void doUpdate( RenderQueueArray & queues )override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdateFlags
		 */
		void doUpdateFlags( PassFlags & passFlags
			, TextureChannels & textureFlags
			, ProgramFlags & programFlags
			, SceneFlags & sceneFlags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doFillTextureDescriptor
		 */
		C3D_API void doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, BillboardListRenderNode & nodes
			, ShadowMapLightTypeArray const & shadowMaps )override;
		/**
		 *\copydoc		castor3d::RenderPass::doFillTextureDescriptor
		 */
		C3D_API void doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, SubmeshRenderNode & nodes
			, ShadowMapLightTypeArray const & shadowMaps )override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdatePipeline
		 */
		C3D_API void doUpdatePipeline( RenderPipeline & pipeline)const override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateDepthStencilState
		 */
		ashes::DepthStencilState doCreateDepthStencilState( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateBlendState
		 */
		ashes::ColourBlendState doCreateBlendState( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetVertexShaderSource
		 */
		C3D_API glsl::Shader doGetVertexShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, bool invertNormals )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetGeometryShaderSource
		 */
		C3D_API glsl::Shader doGetGeometryShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetLegacyPixelShaderSource
		 */
		C3D_API glsl::Shader doGetLegacyPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ashes::CompareOp alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrMRPixelShaderSource
		 */
		C3D_API glsl::Shader doGetPbrMRPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ashes::CompareOp alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrSGPixelShaderSource
		 */
		glsl::Shader doGetPbrSGPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ashes::CompareOp alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPixelShaderSource
		 */
		C3D_API glsl::Shader doGetPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ashes::CompareOp alphaFunc )const;

	private:
		ashes::RenderPassPtr m_renderPass;
		ashes::FrameBufferPtr m_frameBuffer;
	};
}

#endif
