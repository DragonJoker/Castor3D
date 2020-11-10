/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderTechniquePass_H___
#define ___C3D_RenderTechniquePass_H___

#include "TechniqueModule.hpp"

#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/RenderPass.hpp"

#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	class RenderTechniquePass
		: public RenderPass
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor for opaque nodes.
		 *\param[in]	category	The pass category.
		 *\param[in]	name		The pass name.
		 *\param[in]	matrixUbo	The scene matrices UBO.
		 *\param[in]	culler		The culler for this pass.
		 *\param[in]	environment	Pass used for an environment map rendering.
		 *\param[in]	ignored		The geometries attached to this node will be ignored in the render.
		 *\param[in]	config		The SSAO configuration.
		 *\~french
		 *\brief		Constructeur pour les noeuds opaques.
		 *\param[in]	category	La catégorie de la passe.
		 *\param[in]	name		Le nom de la passe.
		 *\param[in]	matrixUbo	L'UBO de matrices de la scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 *\param[in]	environment	Passe utilisée pour le rendu d'une texture d'environnement.
		 *\param[in]	ignored		Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 *\param[in]	config		La configuration du SSAO.
		 */
		C3D_API RenderTechniquePass( castor::String const & category
			, castor::String const & name
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, bool environment
			, SceneNode const * ignored
			, SsaoConfig const & config );
		/**
		 *\~english
		 *\brief		Constructor for transparent nodes.
		 *\param[in]	category	The pass category.
		 *\param[in]	name		The technique name.
		 *\param[in]	matrixUbo	The scene matrices UBO.
		 *\param[in]	culler		The culler for this pass.
		 *\param[in]	oit			The OIT status.
		 *\param[in]	environment	Pass used for an environment map rendering.
		 *\param[in]	ignored		The geometries attached to this node will be ignored in the render.
		 *\param[in]	config		The SSAO configuration.
		 *\~french
		 *\brief		Constructeur pour les noeuds transparents.
		 *\param[in]	category	La catégorie de la passe.
		 *\param[in]	name		Le nom de la technique.
		 *\param[in]	matrixUbo	L'UBO de matrices de la scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 *\param[in]	oit			Le statut d'OIT.
		 *\param[in]	environment	Passe utilisée pour le rendu d'une texture d'environnement.
		 *\param[in]	ignored		Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 *\param[in]	config		La configuration du SSAO.
		 */
		C3D_API RenderTechniquePass( castor::String const & category
			, castor::String const & name
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, bool oit
			, bool environment
			, SceneNode const * ignored
			, SsaoConfig const & config );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~RenderTechniquePass();
		/**
		 *\~english
		 *\brief		Visitor acceptance function.
		 *\~french
		 *\brief		Fonction d'acceptation de visiteur.
		 */
		C3D_API virtual void accept( RenderTechniqueVisitor & visitor );
		/**
		 *\~english
		 *\brief		Render function.
		 *\param[out]	info		Receives the render informations.
		 *\param[in]	jitter		The jittering value.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[out]	info		Reçoit les informations de rendu.
		 *\param[in]	jitter		La valeur de jittering.
		 */
		C3D_API void update( GpuUpdater & updater );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline ashes::Semaphore const & getSemaphore()const
		{
			CU_Require( m_finished );
			return *m_finished;
		}
		/**@}*/

	public:
		using RenderPass::update;

	protected:
		/**
		 *\copydoc		castor3d::RenderPass::doCleanup
		 */
		C3D_API virtual void doCleanup( RenderDevice const & device )override;

	private:
		/**
		 *\~english
		 *\brief			Renders render nodes.
		 *\param[in]		nodes		The scene render nodes.
		 *\param			camera		The viewing camera.
		 *\param[in,out]	count		Receives the rendered nodes count.
		 *\~french
		 *\brief			Dessine les noeuds de rendu.
		 *\param[in]		nodes		Les noeuds de rendu de la scène.
		 *\param			camera		La caméra regardant la scène.
		 *\param[in,out]	count		Reçouit le nombre de noeuds dessinés.
		 */
		C3D_API virtual void doUpdateNodes( SceneCulledRenderNodes & nodes
			, castor::Point2f const & jitter
			, RenderInfo & info );
		/**
		 *\copydoc		castor3d::RenderPass::doUpdateUbos
		 */
		C3D_API void doUpdateUbos( CpuUpdater & updater )override;
		/**
		 *\copydoc		castor3d::RenderPass::doInitialise
		 */
		C3D_API bool doInitialise( RenderDevice const & device
			, castor::Size const & size )override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdateFlags
		 */
		C3D_API void doUpdateFlags( PipelineFlags & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetGeometryShaderSource
		 */
		C3D_API ShaderPtr doGetGeometryShaderSource( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doFillUboDescriptor
		 */
		C3D_API void doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
			, BillboardListRenderNode & nodes )override;
		/**
		 *\copydoc		castor3d::RenderPass::doFillUboDescriptor
		 */
		C3D_API void doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
			, SubmeshRenderNode & nodes )override;
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
		C3D_API void doUpdatePipeline( RenderPipeline & pipeline )override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateDepthStencilState
		 */
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateBlendState
		 */
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;

	protected:
		/**
		 *\copydoc		castor3d::RenderPass::doCreateTextureBindings
		 */
		ashes::VkDescriptorSetLayoutBindingArray doCreateTextureBindings( PipelineFlags const & flags )const override;

	protected:
		Scene const & m_scene;
		Camera * m_camera{ nullptr };
		SceneRenderNode m_sceneNode;
		bool m_environment{ false };
		SsaoConfig m_ssaoConfig;
		ashes::SemaphorePtr m_finished;
	};
}

#endif
