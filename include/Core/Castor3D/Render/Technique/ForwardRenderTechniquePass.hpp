/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ForwardRenderTechniquePass_H___
#define ___C3D_ForwardRenderTechniquePass_H___

#include "TechniqueModule.hpp"

#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"

namespace castor3d
{
	class ForwardRenderTechniquePass
		: public RenderTechniquePass
	{
	public:
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
		C3D_API ForwardRenderTechniquePass( RenderDevice const & device
			, castor::String const & category
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
		C3D_API ForwardRenderTechniquePass( RenderDevice const & device
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
		C3D_API ForwardRenderTechniquePass( RenderDevice const & device
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
		C3D_API ForwardRenderTechniquePass( RenderDevice const & device
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
		/**
		 *\~english
		 *\brief		Initialises the render pass.
		 *\param[in]	device		The GPU device.
		 *\param[in]	colourView	The target colour view.
		 *\param[in]	depthView	The target depth view.
		 *\param[in]	size		The pass dimensions.
		 *\param[in]	clear		Tells if the attaches must be cleared at the beginning of the pass.
		 *\~french
		 *\brief		Initialise la passe de rendu.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	colourView	La vue couleur cible.
		 *\param[in]	depthView	La vue profondeur cible.
		 *\param[in]	size		Les dimensions de la passe.
		 *\param[in]	clear		Dit si les attaches doivent être vidées au début de la passe.
		 */
		C3D_API void initialiseRenderPass( ashes::ImageView const & colourView
			, ashes::ImageView const & depthView
			, castor::Size const & size
			, bool clear );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor )override;
		/**
		 *\~english
		 *\brief		Renders nodes.
		 *\param[in]	device	The GPU device.
		 *\param[out]	toWait	The semaphore to wait for.
		 *\~french
		 *\brief		Dessine les noeuds.
		 *\param[in]	device	Le device GPU.
		 *\param[out]	toWait	Le sémaphore à attendre.
		 */
		C3D_API ashes::Semaphore const & render( ashes::Semaphore const & toWait );

	protected:
		/**
		 *\copydoc		castor3d::RenderTechniquePass::doCleanup
		 */
		C3D_API void doCleanup()override;

	private:
		ShaderPtr doGetPhongPixelShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const override;

	protected:
		ashes::FrameBufferPtr m_frameBuffer;
		ashes::CommandBufferPtr m_nodesCommands;
	};
}

#endif
