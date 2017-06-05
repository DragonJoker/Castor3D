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
#ifndef ___C3D_ForwardRenderTechniquePass_H___
#define ___C3D_ForwardRenderTechniquePass_H___

#include "RenderTechniquePass.hpp"

#include "ShadowMap/ShadowMapDirectional.hpp"
#include "ShadowMap/ShadowMapPoint.hpp"
#include "ShadowMap/ShadowMapSpot.hpp"

namespace Castor3D
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
		 *\brief		Constructor
		 *\param[in]	p_name			The technique name.
		 *\param[in]	p_scene			The scene for this technique.
		 *\param[in]	p_camera		The camera for this technique (may be null).
		 *\param[in]	p_opaque		Tells if this pass if for opaque nodes.
		 *\param[in]	p_multisampling	The multisampling status.
		 *\param[in]	p_environment	Pass used for an environment map rendering.
		 *\param[in]	p_ignored		The geometries attached to this node will be ignored in the render.
		 *\param[in]	p_config		The SSAO configuration.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name			Le nom de la technique.
		 *\param[in]	p_scene			La scène pour cette technique.
		 *\param[in]	p_camera		La caméra pour cette technique (peut être nulle).
		 *\param[in]	p_opaque		Dit si cette passe de rendu est pour les noeuds opaques.
		 *\param[in]	p_multisampling	Le statut de multiéchantillonnage.
		 *\param[in]	p_environment	Passe utilisée pour le rendu d'une texture d'environnement.
		 *\param[in]	p_ignored		Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 *\param[in]	p_config		La configuration du SSAO.
		 */
		C3D_API ForwardRenderTechniquePass( Castor::String const & p_name
			, Scene & p_scene
			, Camera * p_camera
			, bool p_opaque
			, bool p_multisampling
			, bool p_environment
			, SceneNode const * p_ignored
			, SsaoConfig const & p_config );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~ForwardRenderTechniquePass();
		/**
		 *\~english
		 *\brief		Render function.
		 *\param[out]	p_info		Receives the render informations.
		 *\param[out]	p_shadows	Tells if the scene has shadow producing light sources.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[out]	p_info		Reçoit les informations de rendu.
		 *\param[out]	p_shadows	Dit si la scène a des lumières produisant des ombres.
		 */
		C3D_API void Render( RenderInfo & p_info, bool p_shadows )override;
		/**
		 *\copydoc		Castor3D::RenderTechniquePass::InitialiseShadowMaps
		 */
		C3D_API bool InitialiseShadowMaps()override;
		/**
		 *\copydoc		Castor3D::RenderTechniquePass::CleanupShadowMaps
		 */
		C3D_API void CleanupShadowMaps()override;
		/**
		 *\copydoc		Castor3D::RenderTechniquePass::UpdateShadowMaps
		 */
		C3D_API void UpdateShadowMaps( RenderQueueArray & p_queues )override;
		/**
		 *\copydoc		Castor3D::RenderTechniquePass::RenderShadowMaps
		 */
		C3D_API void RenderShadowMaps()override;

	private:
		/**
		 *\copydoc		Castor3D::RenderTechniquePass::DoGetDepthMaps
		 */
		C3D_API void DoGetDepthMaps( DepthMapArray & p_depthMaps )override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetVertexShaderSource
		 */
		C3D_API Castor::String DoGetVertexShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, SceneFlags const & p_sceneFlags
			, bool p_invertNormals )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetPixelShaderSource
		 */
		C3D_API Castor::String DoGetPixelShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, SceneFlags const & p_sceneFlags )const override;

	private:
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
