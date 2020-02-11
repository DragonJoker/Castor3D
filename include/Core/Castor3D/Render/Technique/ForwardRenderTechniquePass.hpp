/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ForwardRenderTechniquePass_H___
#define ___C3D_ForwardRenderTechniquePass_H___

#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"

#include "Castor3D/Render/ShadowMap/ShadowMapDirectional.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPoint.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapSpot.hpp"

namespace castor3d
{
	class ForwardRenderTechniquePass
		: public RenderTechniquePass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor for opaque nodes.
		 *\param[in]	name		The technique name.
		 *\param[in]	matrixUbo	The scene matrices UBO.
		 *\param[in]	culler		The culler for this pass.
		 *\param[in]	environment	Pass used for an environment map rendering.
		 *\param[in]	ignored		The geometries attached to this node will be ignored in the render.
		 *\param[in]	config		The SSAO configuration.
		 *\~french
		 *\brief		Constructeur pour les noeuds opaques.
		 *\param[in]	name		Le nom de la technique.
		 *\param[in]	matrixUbo	L'UBO de matrices de la scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 *\param[in]	environment	Passe utilisée pour le rendu d'une texture d'environnement.
		 *\param[in]	ignored		Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 *\param[in]	config		La configuration du SSAO.
		 */
		C3D_API ForwardRenderTechniquePass( castor::String const & name
			, MatrixUbo const & matrixUbo
			, SceneCuller & culler
			, bool environment
			, SceneNode const * ignored
			, SsaoConfig const & config );
		/**
		 *\~english
		 *\brief		Constructor for transparent nodes.
		 *\param[in]	name		The technique name.
		 *\param[in]	matrixUbo	The scene matrices UBO.
		 *\param[in]	culler		The culler for this pass.
		 *\param[in]	oit			The OIT status.
		 *\param[in]	environment	Pass used for an environment map rendering.
		 *\param[in]	ignored		The geometries attached to this node will be ignored in the render.
		 *\param[in]	config		The SSAO configuration.
		 *\~french
		 *\brief		Constructeur pour les noeuds transparents.
		 *\param[in]	name		Le nom de la technique.
		 *\param[in]	matrixUbo	L'UBO de matrices de la scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 *\param[in]	oit			Le statut d'OIT.
		 *\param[in]	environment	Passe utilisée pour le rendu d'une texture d'environnement.
		 *\param[in]	ignored		Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 *\param[in]	config		La configuration du SSAO.
		 */
		C3D_API ForwardRenderTechniquePass( castor::String const & name
			, MatrixUbo const & matrixUbo
			, SceneCuller & culler
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
		 *\~english
		 *\brief		Initialises the render pass.
		 *\param[in]	colourView	The target colour view.
		 *\param[in]	depthView	The target depth view.
		 *\param[in]	size		The pass dimensions.
		 *\param[in]	clear		Tells if the attaches must be cleared at the beginning of the pass.
		 *\~french
		 *\brief		Initialise la passe de rendu.
		 *\param[in]	colourView	La vue couleur cible.
		 *\param[in]	depthView	La vue profondeur cible.
		 *\param[in]	size		Les dimensions de la passe.
		 *\param[in]	clear		Dit si les attaches doivent être vidées au début de la passe.
		 */
		void initialiseRenderPass( ashes::ImageView const & colourView
			, ashes::ImageView const & depthView
			, castor::Size const & size
			, bool clear );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor )override;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::update
		 */
		C3D_API void update( RenderInfo & info
			, castor::Point2f const & jitter )override;
		/**
		 *\~english
		 *\brief		Renders nodes.
		 *\param[out]	toWait	The semaphore to wait for.
		 *\~french
		 *\brief		Dessine les noeuds.
		 *\param[out]	toWait	Le sémaphore à attendre.
		 */
		ashes::Semaphore const & render( ashes::Semaphore const & toWait );

	protected:
		/**
		 *\copydoc		castor3d::RenderTechniquePass::doCleanup
		 */
		C3D_API virtual void doCleanup()override;

	private:
		/**
		 *\copydoc		castor3d::RenderPass::doCreateTextureBindings
		 */
		C3D_API ashes::VkDescriptorSetLayoutBindingArray doCreateTextureBindings( PipelineFlags const & flags )const override;
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
		 *\copydoc		castor3d::RenderPass::doGetVertexShaderSource
		 */
		C3D_API ShaderPtr doGetVertexShaderSource( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPhongPixelShaderSource
		 */
		C3D_API ShaderPtr doGetPhongPixelShaderSource( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrMRPixelShaderSource
		 */
		C3D_API ShaderPtr doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrSGPixelShaderSource
		 */
		C3D_API ShaderPtr doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const override;

	protected:
		ashes::FrameBufferPtr m_frameBuffer;
		ashes::CommandBufferPtr m_nodesCommands;
	};
}

#endif
