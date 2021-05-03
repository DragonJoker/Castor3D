/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderTechniquePass_H___
#define ___C3D_RenderTechniquePass_H___

#include "TechniqueModule.hpp"

#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/RenderPass.hpp"

#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	class RenderTechniquePass
		: public SceneRenderPass
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor for opaque nodes.
		 *\param[in]	category		The pass category.
		 *\param[in]	name			The pass name.
		 *\param[in]	matrixUbo		The scene matrices UBO.
		 *\param[in]	culler			The culler for this pass.
		 *\param[in]	environment		Pass used for an environment map rendering.
		 *\param[in]	ignored			The geometries attached to this node will be ignored in the render.
		 *\param[in]	ssaoConfig		The SSAO configuration.
		 *\param[in]	lpvConfigUbo	The LPV configuration, if needed.
		 *\param[in]	llpvConfigUbo	The Layered LPV configuration, if needed.
		 *\param[in]	vctConfigUbo	The VCT configuration, if needed.
		 *\~french
		 *\brief		Constructeur pour les noeuds opaques.
		 *\param[in]	category		La catégorie de la passe.
		 *\param[in]	name			Le nom de la passe.
		 *\param[in]	matrixUbo		L'UBO de matrices de la scène.
		 *\param[in]	culler			Le culler pour cette passe.
		 *\param[in]	environment		Passe utilisée pour le rendu d'une texture d'environnement.
		 *\param[in]	ignored			Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 *\param[in]	ssaoConfig		La configuration du SSAO.
		 *\param[in]	lpvConfigUbo	La configuration des LPV, si nécessaire.
		 *\param[in]	llpvConfigUbo	La configuration des Layered LPV, si nécessaire.
		 *\param[in]	vctConfigUbo	La configuration du VCT, si nécessaire.
		 */
		C3D_API RenderTechniquePass( castor::String const & category
			, castor::String const & name
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, bool environment
			, SceneNode const * ignored
			, SsaoConfig const & ssaoConfig
			, LpvGridConfigUbo const * lpvConfigUbo = nullptr
			, LayeredLpvGridConfigUbo const * llpvConfigUbo = nullptr
			, VoxelizerUbo const * vctConfigUbo = nullptr );
		/**
		 *\~english
		 *\brief		Constructor for transparent nodes.
		 *\param[in]	category		The pass category.
		 *\param[in]	name			The technique name.
		 *\param[in]	matrixUbo		The scene matrices UBO.
		 *\param[in]	culler			The culler for this pass.
		 *\param[in]	oit				The OIT status.
		 *\param[in]	environment		Pass used for an environment map rendering.
		 *\param[in]	ignored			The geometries attached to this node will be ignored in the render.
		 *\param[in]	ssaoConfig		The SSAO configuration.
		 *\param[in]	lpvConfigUbo	The LPV configuration, if needed.
		 *\param[in]	llpvConfigUbo	The Layered LPV configuration, if needed.
		 *\param[in]	vctConfigUbo	The VCT configuration, if needed.
		 *\~french
		 *\brief		Constructeur pour les noeuds transparents.
		 *\param[in]	category		La catégorie de la passe.
		 *\param[in]	name			Le nom de la technique.
		 *\param[in]	matrixUbo		L'UBO de matrices de la scène.
		 *\param[in]	culler			Le culler pour cette passe.
		 *\param[in]	oit				Le statut d'OIT.
		 *\param[in]	environment		Passe utilisée pour le rendu d'une texture d'environnement.
		 *\param[in]	ignored			Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 *\param[in]	ssaoConfig		La configuration du SSAO.
		 *\param[in]	lpvConfigUbo	La configuration des LPV, si nécessaire.
		 *\param[in]	llpvConfigUbo	La configuration des Layered LPV, si nécessaire.
		 *\param[in]	vctConfigUbo	La configuration du VCT, si nécessaire.
		 */
		C3D_API RenderTechniquePass( castor::String const & category
			, castor::String const & name
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, bool oit
			, bool environment
			, SceneNode const * ignored
			, SsaoConfig const & ssaoConfig
			, LpvGridConfigUbo const * lpvConfigUbo = nullptr
			, LayeredLpvGridConfigUbo const * llpvConfigUbo = nullptr
			, VoxelizerUbo const * vctConfigUbo = nullptr );
		/**
		 *\~english
		 *\brief		Constructor for opaque nodes.
		 *\param[in]	category		The pass category.
		 *\param[in]	name			The pass name.
		 *\param[in]	matrixUbo		The scene matrices UBO.
		 *\param[in]	culler			The culler for this pass.
		 *\param[in]	environment		Pass used for an environment map rendering.
		 *\param[in]	ignored			The geometries attached to this node will be ignored in the render.
		 *\param[in]	ssaoConfig		The SSAO configuration.
		 *\param[in]	lpvConfigUbo	The LPV configuration, if needed.
		 *\param[in]	llpvConfigUbo	The Layered LPV configuration, if needed.
		 *\param[in]	vctConfigUbo	The VCT configuration, if needed.
		 *\~french
		 *\brief		Constructeur pour les noeuds opaques.
		 *\param[in]	category		La catégorie de la passe.
		 *\param[in]	name			Le nom de la passe.
		 *\param[in]	matrixUbo		L'UBO de matrices de la scène.
		 *\param[in]	culler			Le culler pour cette passe.
		 *\param[in]	environment		Passe utilisée pour le rendu d'une texture d'environnement.
		 *\param[in]	ignored			Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 *\param[in]	ssaoConfig		La configuration du SSAO.
		 *\param[in]	lpvConfigUbo	La configuration des LPV, si nécessaire.
		 *\param[in]	llpvConfigUbo	La configuration des Layered LPV, si nécessaire.
		 *\param[in]	vctConfigUbo	La configuration du VCT, si nécessaire.
		 */
		C3D_API RenderTechniquePass( RenderDevice const & device
			, castor::Size const & size
			, castor::String const & category
			, castor::String const & name
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, bool environment
			, SceneNode const * ignored
			, SsaoConfig const & ssaoConfig
			, LpvGridConfigUbo const * lpvConfigUbo = nullptr
			, LayeredLpvGridConfigUbo const * llpvConfigUbo = nullptr
			, VoxelizerUbo const * vctConfigUbo = nullptr
			, LightVolumePassResult const * lpvResult = nullptr
			, TextureUnit const * vctFirstBounce = nullptr
			, TextureUnit const * vctSecondaryBounce = nullptr );
		/**
		 *\~english
		 *\brief		Constructor for transparent nodes.
		 *\param[in]	category		The pass category.
		 *\param[in]	name			The technique name.
		 *\param[in]	matrixUbo		The scene matrices UBO.
		 *\param[in]	culler			The culler for this pass.
		 *\param[in]	oit				The OIT status.
		 *\param[in]	environment		Pass used for an environment map rendering.
		 *\param[in]	ignored			The geometries attached to this node will be ignored in the render.
		 *\param[in]	ssaoConfig		The SSAO configuration.
		 *\param[in]	lpvConfigUbo	The LPV configuration, if needed.
		 *\param[in]	llpvConfigUbo	The Layered LPV configuration, if needed.
		 *\param[in]	vctConfigUbo	The VCT configuration, if needed.
		 *\~french
		 *\brief		Constructeur pour les noeuds transparents.
		 *\param[in]	category		La catégorie de la passe.
		 *\param[in]	name			Le nom de la technique.
		 *\param[in]	matrixUbo		L'UBO de matrices de la scène.
		 *\param[in]	culler			Le culler pour cette passe.
		 *\param[in]	oit				Le statut d'OIT.
		 *\param[in]	environment		Passe utilisée pour le rendu d'une texture d'environnement.
		 *\param[in]	ignored			Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 *\param[in]	ssaoConfig		La configuration du SSAO.
		 *\param[in]	lpvConfigUbo	La configuration des LPV, si nécessaire.
		 *\param[in]	llpvConfigUbo	La configuration des Layered LPV, si nécessaire.
		 *\param[in]	vctConfigUbo	La configuration du VCT, si nécessaire.
		 */
		C3D_API RenderTechniquePass( RenderDevice const & device
			, castor::Size const & size
			, castor::String const & category
			, castor::String const & name
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, bool oit
			, bool environment
			, SceneNode const * ignored
			, SsaoConfig const & ssaoConfig
			, LpvGridConfigUbo const * lpvConfigUbo = nullptr
			, LayeredLpvGridConfigUbo const * llpvConfigUbo = nullptr
			, VoxelizerUbo const * vctConfigUbo = nullptr
			, LightVolumePassResult const * lpvResult = nullptr
			, TextureUnit const * vctFirstBounce = nullptr
			, TextureUnit const * vctSecondaryBounce = nullptr );

	public:
		/**
		 *\~english
		 *\brief		Initialises the pass.
		 *\param[in]	device				The current device.
		 *\param[in]	size				The pass needed dimensions.
		 *\param[in]	lpvResult			The LPV result, if needed.
		 *\param[in]	vctFirstBounce		The VCT first bounce result, if needed.
		 *\param[in]	vctSecondaryBounce	The VCT secondary bounce result, if needed.
		 *\return		\p true on ok.
		 *\~french
		 *\brief		Initialise la passe.
		 *\param[in]	device				Le device actuel.
		 *\param[in]	size				Les dimensions voulues pour la passe.
		 *\param[in]	lpvResult			Le résultat du LPV, si nécessaire.
		 *\param[in]	vctFirstBounce		Le résultat du premier rebond de VCT, si nécessaire.
		 *\param[in]	vctSecondaryBounce	Le résultat du second rebond de VCT, si nécessaire.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool initialise( RenderDevice const & device
			, castor::Size const & size
			, LightVolumePassResult const * lpvResult = nullptr
			, TextureUnit const * vctFirstBounce = nullptr
			, TextureUnit const * vctSecondaryBounce = nullptr );
		/**
		 *\~english
		 *\brief		Initialises the pass.
		 *\param[in]	device				The GPU device.
		 *\param[in]	size				The pass needed dimensions.
		 *\param[in]	timer				The parent timer.
		 *\param[in]	index				The pass timer index, in its parent.
		 *\param[in]	lpvResult			The LPV result, if needed.
		 *\param[in]	vctFirstBounce		The VCT first bounce result, if needed.
		 *\param[in]	vctSecondaryBounce	The VCT secondary bounce result, if needed.
		 *\return		\p true on ok.
		 *\~french
		 *\brief		Initialise la passe.
		 *\param[in]	device				Le device GPU.
		 *\param[in]	size				Les dimensions voulues pour la passe.
		 *\param[in]	timer				Le timer parent.
		 *\param[in]	index				L'indice de la passe, dans le parent.
		 *\param[in]	lpvResult			Le résultat du LPV, si nécessaire.
		 *\param[in]	vctFirstBounce		Le résultat du premier rebond de VCT, si nécessaire.
		 *\param[in]	vctSecondaryBounce	Le résultat du second rebond de VCT, si nécessaire.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool initialise( RenderDevice const & device
			, castor::Size const & size
			, RenderPassTimer & timer
			, uint32_t index
			, LightVolumePassResult const * lpvResult = nullptr
			, TextureUnit const * vctFirstBounce = nullptr
			, TextureUnit const * vctSecondaryBounce = nullptr );
		/**
		 *\~english
		 *\brief		Visitor acceptance function.
		 *\param		visitor	The ... visitor.
		 *\~french
		 *\brief		Fonction d'acceptation de visiteur.
		 *\param		visitor	Le ... visiteur.
		 */
		C3D_API virtual void accept( RenderTechniqueVisitor & visitor );
		/**
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
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

		C3D_API ShaderFlags getShaderFlags()const override
		{
			return ShaderFlag::eWorldSpace
				| ShaderFlag::eTangentSpace
				| ShaderFlag::eVelocity
				| ShaderFlag::eViewSpace;
		}
		/**@}*/

	public:
		using SceneRenderPass::update;

	private:
		C3D_API virtual void doUpdateNodes( SceneCulledRenderNodes & nodes
			, castor::Point2f const & jitter
			, RenderInfo & info );
		C3D_API void doUpdateUbos( CpuUpdater & updater )override;
		C3D_API bool doInitialise( RenderDevice const & device
			, castor::Size const & size )override;
		C3D_API void doUpdateFlags( PipelineFlags & flags )const override;
		C3D_API ShaderPtr doGetGeometryShaderSource( PipelineFlags const & flags )const override;
		C3D_API void doFillUboDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
			, BillboardListRenderNode & nodes )override;
		C3D_API void doFillUboDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
			, SubmeshRenderNode & nodes )override;
		C3D_API void doFillTextureDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, BillboardListRenderNode & nodes
			, ShadowMapLightTypeArray const & shadowMaps )override;
		C3D_API void doFillTextureDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, SubmeshRenderNode & nodes
			, ShadowMapLightTypeArray const & shadowMaps )override;
		C3D_API void doUpdatePipeline( RenderPipeline & pipeline )override;
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;

	protected:
		/**
		 *\~english
		 *\brief		Cleans up the pass.
		 *\~french
		 *\brief		Nettoie la passe.
		 */
		C3D_API virtual void doCleanup( RenderDevice const & device )override;
		/**
		 *\copydoc		castor3d::SceneRenderPass::doCreateUboBindings
		 */
		ashes::VkDescriptorSetLayoutBindingArray doCreateUboBindings( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::SceneRenderPass::doCreateTextureBindings
		 */
		ashes::VkDescriptorSetLayoutBindingArray doCreateTextureBindings( PipelineFlags const & flags )const override;
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source matching the given flags.
		 *\param[in]	flags	The pipeline flags.
		 *\~french
		 *\brief		Récupère le source du vertex shader qui correspond aux indicateurs donnés.
		 *\param[in]	flags	Les indicateurs de pipeline.
		 */
		ShaderPtr doGetVertexShaderSource( PipelineFlags const & flags )const override;

	private:
		using SceneRenderPass::initialise;

	protected:
		Scene const & m_scene;
		LpvGridConfigUbo const * m_lpvConfigUbo;
		LayeredLpvGridConfigUbo const * m_llpvConfigUbo;
		VoxelizerUbo const * m_vctConfigUbo;
		LightVolumePassResult const * m_lpvResult;
		TextureUnit const * m_vctFirstBounce;
		TextureUnit const * m_vctSecondaryBounce;
		Camera * m_camera{ nullptr };
		SceneRenderNode m_sceneNode;
		bool m_environment{ false };
		SsaoConfig m_ssaoConfig;
		ashes::SemaphorePtr m_finished;
	};
}

#endif
