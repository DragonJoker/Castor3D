/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_RenderTechniquePass_H___
#define ___C3D_RenderTechniquePass_H___

#include "Miscellaneous/SsaoConfig.hpp"
#include "Render/RenderPass.hpp"

namespace Castor3D
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
		C3D_API RenderTechniquePass( Castor::String const & name
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
		C3D_API RenderTechniquePass( Castor::String const & name
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
		 *\param[out]	info	Receives the render informations.
		 *\param[out]	shadows	Tells if the scene has shadow producing light sources.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[out]	info	Reçoit les informations de rendu.
		 *\param[out]	shadows	Dit si la scène a des lumières produisant des ombres.
		 */
		C3D_API virtual void Render( RenderInfo & info, bool shadows ) = 0;
		/**
		 *\~english
		 *\return		Initialises the shadow maps.
		 *\~french
		 *\return		Initialise les maps de'ombres.
		 */
		C3D_API virtual bool InitialiseShadowMaps() = 0;
		/**
		 *\~english
		 *\return		Cleans up the shadow maps.
		 *\~french
		 *\return		Nettoie les maps de'ombres.
		 */
		C3D_API virtual void CleanupShadowMaps() = 0;
		/**
		 *\~english
		 *\brief		Updates the shadow maps.
		 *\remarks		Gather the render queues, for further update.
		 *\param[out]	queues	Receives the render queues needed for the rendering of the frame.
		 *\~french
		 *\brief		Met à jour les maps d'ombres.
		 *\remarks		Récupère les files de rendu, pour mise à jour ultérieure.
		 *\param[out]	queues	Reçoit les files de rendu nécessaires pour le dessin de la frame.
		 */
		C3D_API virtual void UpdateShadowMaps( RenderQueueArray & queues ) = 0;
		/**
		 *\~english
		 *\brief		Renders the shadow maps.
		 *\~french
		 *\brief		Dessine les maps d'ombres.
		 */
		C3D_API virtual void RenderShadowMaps() = 0;
		/**
		 *\~english
		 *\brief		Adds a shadow producing light source.
		 *\param[in]	light	The light source.
		 *\~french
		 *\brief		Ajoute une source lumineuse produisant des ombres.
		 *\param[in]	light	La source lumineuse.
		 */
		C3D_API virtual void AddShadowProducer( Light & light ) = 0;

	protected:
		/**
		 *\~english
		 *\brief		Render function.
		 *\param[out]	info	Receives the render informations.
		 *\param[out]	shadows	Tells if the scene has shadow producing light sources.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[out]	info	Reçoit les informations de rendu.
		 *\param[out]	shadows	Dit si la scène a des lumières produisant des ombres.
		 */
		C3D_API void DoRender( RenderInfo & info, bool shadows );
		/**
		 *\~english
		 *\brief			Renders render nodes.
		 *\param[in]		p_nodes		The scene render nodes.
		 *\param			camera		The viewing camera.
		 *\param[in]		depthMaps	The depth (shadows and other) maps.
		 *\param[in, out]	count		Receives the rendered nodes count.
		 *\~french
		 *\brief			Dessine les noeuds de rendu.
		 *\param[in]		p_nodes		Les noeuds de rendu de la scène.
		 *\param			camera		La caméra regardant la scène.
		 *\param[in]		depthMaps	Les textures de profondeur (ombres et autres).
		 *\param[in, out]	count		Reçouit le nombre de noeuds dessinés.
		 */
		C3D_API void DoRenderNodes( SceneRenderNodes & p_nodes
			, Camera const & camera
			, DepthMapArray & depthMaps
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Retrieves the depth maps for opaque nodes.
		 *\param[out]	depthMaps	Receives the depth maps.
		 *\~french
		 *\brief		Récupère les textures de profondeur pour les noeuds opaques.
		 *\param[out]	depthMaps	Reçoit les textures.
		 */
		C3D_API virtual void DoGetDepthMaps( DepthMapArray & depthMaps ) = 0;
		/**
		 *\copydoc		Castor3D::RenderPass::DoInitialise
		 */
		C3D_API bool DoInitialise( Castor::Size const & size )override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoCleanup
		 */
		C3D_API void DoCleanup()override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoUpdate
		 */
		C3D_API void DoUpdate( RenderQueueArray & queues )override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoUpdateFlags
		 */
		C3D_API void DoUpdateFlags( TextureChannels & textureFlags
			, ProgramFlags & programFlags
			, SceneFlags & sceneFlags )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetGeometryShaderSource
		 */
		C3D_API GLSL::Shader DoGetGeometryShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoUpdatePipeline
		 */
		C3D_API void DoUpdatePipeline( RenderPipeline & pipeline )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoPrepareFrontPipeline
		 */
		C3D_API void DoPrepareFrontPipeline( ShaderProgram & program
			, PipelineFlags const & flags )override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoPrepareBackPipeline
		 */
		C3D_API void DoPrepareBackPipeline( ShaderProgram & program
			, PipelineFlags const & flags )override;

	protected:
		//!\~english	The rendered scne.
		//!\~french		La scène rendue.
		Scene & m_scene;
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
