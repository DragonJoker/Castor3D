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
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name			The technique name.
		 *\param[in]	p_renderTarget	The render target for this technique.
		 *\param[in]	p_technique		The parent render technique.
		 *\param[in]	p_opaque		Tells if this pass if for opaque nodes.
		 *\param[in]	p_multisampling	The multisampling status
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name			Le nom de la technique.
		 *\param[in]	p_renderTarget	La render target pour cette technique.
		 *\param[in]	p_technique		La technique de rendu parente.
		 *\param[in]	p_opaque		Dit si cette passe de rendu est pour les noeuds opaques.
		 *\param[in]	p_multisampling	Le statut de multiéchantillonnage.
		 */
		C3D_API RenderTechniquePass( Castor::String const & p_name
			, RenderTarget & p_renderTarget
			, RenderTechnique & p_technique
			, TextureUnit && p_directionalShadowMap
			, std::vector< TextureUnit > && p_pointShadowMaps
			, TextureUnit && p_spotShadowMap
			, bool p_opaque
			, bool p_multisampling = false );
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
		 *\param[out]	p_visible	The visible objects count.
		 *\param[out]	p_shadows	Tells if the scene has shadow producing light sources.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[out]	p_visible	Le nombre d'objets visibles.
		 *\param[out]	p_shadows	Dit si la scène a des lumières produisant des ombres.
		 */
		C3D_API void Render( uint32_t & p_visible, bool p_shadows );
		/**
		 *\~english
		 *\return		Initialises the shadow maps.
		 *\~french
		 *\return		Initialise les maps de'ombres.
		 */
		C3D_API bool InitialiseShadowMaps();
		/**
		 *\~english
		 *\return		Cleans up the shadow maps.
		 *\~french
		 *\return		Nettoie les maps de'ombres.
		 */
		C3D_API void CleanupShadowMaps();
		/**
		 *\~english
		 *\brief		Updates the shadow maps.
		 *\remarks		Gather the render queues, for further update.
		 *\param[out]	p_queues	Receives the render queues needed for the rendering of the frame.
		 *\~french
		 *\brief		Met à jour les maps d'ombres.
		 *\remarks		Récupère les files de rendu, pour mise à jour ultérieure.
		 *\param[out]	p_queues	Reçoit les files de rendu nécessaires pour le dessin de la frame.
		 */
		C3D_API void UpdateShadowMaps( RenderQueueArray & p_queues );
		/**
		 *\~english
		 *\brief		Renders the shadow maps.
		 *\~french
		 *\brief		Dessine les maps d'ombres.
		 */
		C3D_API virtual void RenderShadowMaps() = 0;
		/**
		 *\~english
		 *\return		The directional light shadow map.
		 *\~french
		 *\return		La map d'ombre de la lumière directionnelle.
		 */
		inline ShadowMapDirectional & GetDirectionalShadowMap()
		{
			return m_directionalShadowMap;
		}
		/**
		 *\~english
		 *\return		The spot lights shadow map.
		 *\~french
		 *\return		La map d'ombre des lumières projecteur.
		 */
		inline ShadowMapSpot & GetSpotShadowMap()
		{
			return m_spotShadowMap;
		}
		/**
		 *\~english
		 *\return		The point lights shadow map.
		 *\~french
		 *\return		La map d'ombre des lumières ponctuelles.
		 */
		inline ShadowMapPoint & GetPointShadowMaps()
		{
			return m_pointShadowMap;
		}

	private:
		/**
		 *\~english
		 *\brief			Renders render nodes.
		 *\param[in]		p_nodes		The scene render nodes.
		 *\param			p_camera	The viewing camera.
		 *\param[in]		p_depthMaps	The depth (shadows and other) maps.
		 *\param[in, out]	p_count		Receives the rendered nodes count.
		 *\~french
		 *\brief			Dessine les noeuds de rendu.
		 *\param[in]		p_nodes		Les noeuds de rendu de la scène.
		 *\param			p_camera	La caméra regardant la scène.
		 *\param[in]		p_depthMaps	Les textures de profondeur (ombres et autres).
		 *\param[in, out]	p_count		Reçouit le nombre de noeuds dessinés.
		 */
		C3D_API void DoRenderNodes( SceneRenderNodes & p_nodes
			, Camera const & p_camera
			, DepthMapArray & p_depthMaps
			, uint32_t & p_count );
		/**
		 *\~english
		 *\brief		Renders objects sorted by distance to camera.
		 *\param[in]	p_nodes		The render nodes.
		 *\param		p_camera	The viewing camera.
		 *\param[in]	p_depthMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine d'objets triés par distance à la caméra.
		 *\param[in]	p_nodes		Les noeuds de rendu.
		 *\param		p_camera	La caméra regardant la scène.
		 *\param[in]	p_depthMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void DoRenderByDistance( DistanceSortedNodeMap & p_nodes
			, Camera const & p_camera
			, DepthMapArray & p_depthMaps );
		/**
		 *\~english
		 *\brief			Renders objects sorted by distance to camera.
		 *\param[in]		p_nodes		The render nodes.
		 *\param			p_camera	The viewing camera.
		 *\param[in]		p_depthMaps	The depth (shadows and other) maps.
		 *\param[in, out]	p_count		Receives the rendered nodes count.
		 *\~french
		 *\brief			Dessine d'objets triés par distance à la caméra.
		 *\param[in]		p_nodes		Les noeuds de rendu.
		 *\param			p_camera	La caméra regardant la scène.
		 *\param[in]		p_depthMaps	Les textures de profondeur (ombres et autres).
		 *\param[in, out]	p_count		Reçouit le nombre de noeuds dessinés.
		 */
		C3D_API void DoRenderByDistance( DistanceSortedNodeMap & p_nodes
			, Camera const & p_camera
			, DepthMapArray & p_depthMaps
			, uint32_t & p_count );
		/**
		 *\~english
		 *\brief		Retrieves the depth maps for opaque nodes.
		 *\param[out]	p_depthMaps	Receives the depth maps.
		 *\~french
		 *\brief		Récupère les textures de profondeur pour les noeuds opaques.
		 *\param[out]	p_depthMaps	Reçoit les textures.
		 */
		C3D_API virtual void DoGetDepthMaps( DepthMapArray & p_depthMaps ) = 0;
		/**
		 *\copydoc		Castor3D::RenderPass::DoInitialise
		 */
		C3D_API bool DoInitialise( Castor::Size const & p_size )override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoCleanup
		 */
		C3D_API void DoCleanup()override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoUpdate
		 */
		C3D_API void DoUpdate( RenderQueueArray & p_queues )override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoUpdateFlags
		 */
		C3D_API void DoUpdateFlags( TextureChannels & p_textureFlags
			, ProgramFlags & p_programFlags
			, SceneFlags & p_sceneFlags )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetGeometryShaderSource
		 */
		C3D_API Castor::String DoGetGeometryShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, SceneFlags const & p_sceneFlags )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetPixelShaderSource
		 */
		C3D_API Castor::String DoGetPixelShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, SceneFlags const & p_sceneFlags )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoUpdatePipeline
		 */
		C3D_API void DoUpdatePipeline( RenderPipeline & p_pipeline)const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoPrepareFrontPipeline
		 */
		C3D_API void DoPrepareFrontPipeline( ShaderProgram & p_program
			, PipelineFlags const & p_flags )override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoPrepareBackPipeline
		 */
		C3D_API void DoPrepareBackPipeline( ShaderProgram & p_program
			, PipelineFlags const & p_flags )override;

	protected:
		//!\~english	The parent render target.
		//!\~french		La cible de rendu parente.
		RenderTarget & m_target;
		//!\~english	The parent render technique.
		//!\~french		La technique de rendu parente.
		RenderTechnique & m_technique;
		//!\~english	The scene render node.
		//!\~french		Le noeud de rendu de la scène.
		SceneRenderNode m_sceneNode;
		//!\~english	The shadow map used for directional lights.
		//!\~french		Le mappage d'ombres utilisée pour les lumières de type directionnelles.
		ShadowMapDirectional m_directionalShadowMap;
		//!\~english	The shadow map used for spot lights.
		//!\~french		Le mappage d'ombres utilisée pour les lumières de type spot.
		ShadowMapSpot m_spotShadowMap;
		//!\~english	The shadow map used for pont lights.
		//!\~french		Le mappage d'ombres utilisée pour les lumières de type point.
		ShadowMapPoint m_pointShadowMap;
	};
}

#endif
