/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredOpaquePass_H___
#define ___C3D_DeferredOpaquePass_H___

#include <Technique/RenderTechniquePass.hpp>

namespace castor3d
{
	class RenderTechnique;
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Deferred lighting Render technique pass.
	\~french
	\brief		Classe de passe de technique de rendu implémentant le Deferred lighting.
	*/
	class OpaquePass
		: public RenderTechniquePass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	scene	The scene for this technique.
		 *\param[in]	camera	The camera for this technique (may be null).
		 *\param[in]	config	The SSAO configuration.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	scene	La scène pour cette technique.
		 *\param[in]	camera	La caméra pour cette technique (peut être nulle).
		 *\param[in]	config	La configuration du SSAO.
		 */
		OpaquePass( Scene & scene
			, Camera * camera
			, SsaoConfig const & config );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~OpaquePass();
		/**
		 *\copydoc		castor3d::RenderTechniquePass::render
		 */
		void initialiseRenderPass( GeometryPassResult const & gpResult );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor )override;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::render
		 */
		void update( RenderInfo & info
			, castor::Point2r const & jitter )override;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline renderer::FrameBuffer const & getFrameBuffer()const
		{
			REQUIRE( m_frameBuffer );
			return *m_frameBuffer;
		}
		/**@}*/

	private:
		/**
		*\copydoc		castor3d::RenderPass::doCreateUboBindings
		*/
		renderer::DescriptorSetLayoutBindingArray doCreateUboBindings( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdateFlags
		 */
		void doUpdateFlags( PassFlags & passFlags
			, TextureChannels & textureFlags
			, ProgramFlags & programFlags
			, SceneFlags & sceneFlags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetVertexShaderSource
		 */
		glsl::Shader doGetVertexShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, bool invertNormals )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetLegacyPixelShaderSource
		 */
		glsl::Shader doGetLegacyPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, renderer::CompareOp alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrMRPixelShaderSource
		 */
		glsl::Shader doGetPbrMRPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, renderer::CompareOp alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrSGPixelShaderSource
		 */
		glsl::Shader doGetPbrSGPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, renderer::CompareOp alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdatePipeline
		 */
		void doUpdatePipeline( RenderPipeline & pipeline )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateDepthStencilState
		 */
		renderer::DepthStencilState doCreateDepthStencilState( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateBlendState
		 */
		renderer::ColourBlendState doCreateBlendState( PipelineFlags const & flags )const override;

	private:
		static castor::String const Output1;
		static castor::String const Output2;
		static castor::String const Output3;
		static castor::String const Output4;
		static castor::String const Output5;

		renderer::FrameBufferPtr m_frameBuffer;
	};
}

#endif
