/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ForwardRenderTechniquePass_H___
#define ___C3D_ForwardRenderTechniquePass_H___

#include "RenderTechniquePass.hpp"

#include "ShadowMap/ShadowMapDirectional.hpp"
#include "ShadowMap/ShadowMapPoint.hpp"
#include "ShadowMap/ShadowMapSpot.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		08/12/2016
	\~english
	\brief		Forward render technique pass class.
	\~french
	\brief		Classe de passe de technique de rendu de type forward.
	*/
	class ForwardRenderTechniquePass
		: public RenderTechniquePass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor for opaque nodes.
		 *\param[in]	name		The technique name.
		 *\param[in]	scene		The scene for this technique.
		 *\param[in]	camera		The camera for this technique (may be null).
		 *\param[in]	environment	Pass used for an environment map rendering.
		 *\param[in]	ignored		The geometries attached to this node will be ignored in the render.
		 *\param[in]	config		The SSAO configuration.
		 *\~french
		 *\brief		Constructeur pour les noeuds opaques.
		 *\param[in]	name		Le nom de la technique.
		 *\param[in]	scene		La scène pour cette technique.
		 *\param[in]	camera		La caméra pour cette technique (peut être nulle).
		 *\param[in]	environment	Passe utilisée pour le rendu d'une texture d'environnement.
		 *\param[in]	ignored		Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 *\param[in]	config		La configuration du SSAO.
		 */
		C3D_API ForwardRenderTechniquePass( castor::String const & name
			, Scene & scene
			, Camera * camera
			, bool environment
			, SceneNode const * ignored
			, SsaoConfig const & config );
		/**
		 *\~english
		 *\brief		Constructor for transparent nodes.
		 *\param[in]	name		The technique name.
		 *\param[in]	scene		The scene for this technique.
		 *\param[in]	camera		The camera for this technique (may be null).
		 *\param[in]	oit			The OIT status.
		 *\param[in]	environment	Pass used for an environment map rendering.
		 *\param[in]	ignored		The geometries attached to this node will be ignored in the render.
		 *\param[in]	config		The SSAO configuration.
		 *\~french
		 *\brief		Constructeur pour les noeuds transparents.
		 *\param[in]	name		Le nom de la technique.
		 *\param[in]	scene		La scène pour cette technique.
		 *\param[in]	camera		La caméra pour cette technique (peut être nulle).
		 *\param[in]	oit			Le statut d'OIT.
		 *\param[in]	environment	Passe utilisée pour le rendu d'une texture d'environnement.
		 *\param[in]	ignored		Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 *\param[in]	config		La configuration du SSAO.
		 */
		C3D_API ForwardRenderTechniquePass( castor::String const & name
			, Scene & scene
			, Camera * camera
			, bool oit
			, bool environment
			, SceneNode const * ignored
			, SsaoConfig const & config );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~ForwardRenderTechniquePass();
		/**
		 *\copydoc		castor3d::RenderTechniquePass::render
		 */
		void initialiseRenderPass( renderer::TextureView const & colourView
			, renderer::TextureView const & depthView
			, castor::Size const & size
			, bool clear );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor )override;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::render
		 */
		C3D_API void update( RenderInfo & info
			, castor::Point2r const & jitter )override;
		/**
		 *\~english
		 *\brief		Renders nodes.
		 *\param[out]	toWait	The semaphore to wait for.
		 *\~french
		 *\brief		Dessine les noeuds.
		 *\param[out]	toWait	Le sémaphore à attendre.
		 */
		renderer::Semaphore const & render( renderer::Semaphore const & toWait );

	protected:
		/**
		 *\copydoc		castor3d::RenderPass::doCleanup
		 */
		C3D_API virtual void doCleanup()override;

	private:
		/**
		 *\copydoc		castor3d::RenderPass::doCreateUboBindings
		 */
		C3D_API renderer::DescriptorSetLayoutBindingArray doCreateUboBindings( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateTextureBindings
		 */
		C3D_API renderer::DescriptorSetLayoutBindingArray doCreateTextureBindings( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doFillTextureDescriptor
		 */
		C3D_API void doFillTextureDescriptor( renderer::DescriptorSetLayout const & layout
			, uint32_t & index
			, BillboardListRenderNode & nodes
			, ShadowMapLightTypeArray const & shadowMaps )override;
		/**
		 *\copydoc		castor3d::RenderPass::doFillTextureDescriptor
		 */
		C3D_API void doFillTextureDescriptor( renderer::DescriptorSetLayout const & layout
			, uint32_t & index
			, SubmeshRenderNode & nodes
			, ShadowMapLightTypeArray const & shadowMaps )override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetVertexShaderSource
		 */
		C3D_API glsl::Shader doGetVertexShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, bool invertNormals )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetLegacyPixelShaderSource
		 */
		C3D_API glsl::Shader doGetLegacyPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, renderer::CompareOp alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrMRPixelShaderSource
		 */
		C3D_API glsl::Shader doGetPbrMRPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, renderer::CompareOp alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrSGPixelShaderSource
		 */
		C3D_API glsl::Shader doGetPbrSGPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, renderer::CompareOp alphaFunc )const override;

	protected:
		renderer::FrameBufferPtr m_frameBuffer;
		renderer::CommandBufferPtr m_nodesCommands;
		renderer::FencePtr m_fence;
	};
}

#endif
