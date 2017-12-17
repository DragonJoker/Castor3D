/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderTechniquePass_H___
#define ___C3D_RenderTechniquePass_H___

#include "Miscellaneous/SsaoConfig.hpp"
#include "Render/RenderPass.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		08/12/2016
	\~english
	\brief		Render technique pass base class.
	\~french
	\brief		Classe de base d'une passe de technique de rendu.
	*/
	class RenderTechniquePass
		: public RenderPass
	{
	protected:
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
		C3D_API RenderTechniquePass( castor::String const & name
			, Scene const & scene
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
		C3D_API RenderTechniquePass( castor::String const & name
			, Scene & scene
			, Camera * camera
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
		 *\brief		Render function.
		 *\param[out]	info		Receives the render informations.
		 *\param[out]	shadowMaps	The shadow maps.
		 *\param[out]	jitter		The jittering value.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[out]	info		Reçoit les informations de rendu.
		 *\param[out]	shadowMaps	Les textures d'ombres.
		 *\param[out]	jitter		La valeur de jittering.
		 */
		C3D_API virtual void render( RenderInfo & info
			, ShadowMapLightTypeArray & shadowMaps
			, castor::Point2r const & jitter ) = 0;

	protected:
		/**
		 *\~english
		 *\brief		Render function.
		 *\param[out]	info		Receives the render informations.
		 *\param[out]	shadowMaps	The shadow maps.
		 *\param[out]	jitter		The jittering value.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[out]	info		Reçoit les informations de rendu.
		 *\param[out]	shadowMaps	Les textures d'ombres.
		 *\param[out]	jitter		La valeur de jittering.
		 */
		C3D_API void doRender( RenderInfo & info
			, ShadowMapLightTypeArray & shadowMaps
			, castor::Point2r const & jitter = castor::Point2r{} );

	private:
		/**
		 *\~english
		 *\brief			Renders render nodes.
		 *\param[in]		p_nodes		The scene render nodes.
		 *\param			camera		The viewing camera.
		 *\param[in]		shadowMaps	The shadows maps.
		 *\param[in, out]	count		Receives the rendered nodes count.
		 *\~french
		 *\brief			Dessine les noeuds de rendu.
		 *\param[in]		p_nodes		Les noeuds de rendu de la scène.
		 *\param			camera		La caméra regardant la scène.
		 *\param[in]		shadowMaps	Les textures d'ombres.
		 *\param[in, out]	count		Reçouit le nombre de noeuds dessinés.
		 */
		C3D_API void doRenderNodes( SceneRenderNodes & p_nodes
			, Camera const & camera
			, ShadowMapLightTypeArray & shadowMaps
			, castor::Point2r const & jitter
			, RenderInfo & info )const;
		/**
		 *\copydoc		castor3d::RenderPass::doInitialise
		 */
		C3D_API bool doInitialise( castor::Size const & size )override;
		/**
		 *\copydoc		castor3d::RenderPass::doCleanup
		 */
		C3D_API void doCleanup()override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdate
		 */
		C3D_API void doUpdate( RenderQueueArray & queues )override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdateFlags
		 */
		C3D_API void doUpdateFlags( PassFlags & passFlags
			, TextureChannels & textureFlags
			, ProgramFlags & programFlags
			, SceneFlags & sceneFlags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetGeometryShaderSource
		 */
		C3D_API glsl::Shader doGetGeometryShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdatePipeline
		 */
		C3D_API void doUpdatePipeline( RenderPipeline & pipeline )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doPrepareFrontPipeline
		 */
		C3D_API void doPrepareFrontPipeline( ShaderProgram & program
			, PipelineFlags const & flags )override;
		/**
		 *\copydoc		castor3d::RenderPass::doPrepareBackPipeline
		 */
		C3D_API void doPrepareBackPipeline( ShaderProgram & program
			, PipelineFlags const & flags )override;

	protected:
		//!\~english	The rendered scne.
		//!\~french		La scène rendue.
		Scene const & m_scene;
		//!\~english	The viewer camera, if any.
		//!\~french		La caméra, s'il y en a une.
		Camera * m_camera{ nullptr };
		//!\~english	The scene render node.
		//!\~french		Le noeud de rendu de la scène.
		SceneRenderNode m_sceneNode;
		//!\~english	Tells if the pass is used for an environment map rendering.
		//!\~french		Dit si la passe est utilisée pour le rendu d'une texture d'environnement.
		bool m_environment{ false };
		//!\~english	The SSAO configuration.
		//!\~french		La configuration du SSAO.
		SsaoConfig m_ssaoConfig;
	};
}

#endif
