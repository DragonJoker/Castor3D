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
		 *\param[in]	p_name			The technique name.
		 *\param[in]	p_scene			The scene for this technique.
		 *\param[in]	p_camera		The camera for this technique (may be null).
		 *\param[in]	p_environment	Pass used for an environment map rendering.
		 *\param[in]	p_ignored		The geometries attached to this node will be ignored in the render.
		 *\param[in]	p_config		The SSAO configuration.
		 *\~french
		 *\brief		Constructeur pour les noeuds opaques.
		 *\param[in]	p_name			Le nom de la technique.
		 *\param[in]	p_scene			La scène pour cette technique.
		 *\param[in]	p_camera		La caméra pour cette technique (peut être nulle).
		 *\param[in]	p_environment	Passe utilisée pour le rendu d'une texture d'environnement.
		 *\param[in]	p_ignored		Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 *\param[in]	p_config		La configuration du SSAO.
		 */
		C3D_API ForwardRenderTechniquePass( castor::String const & p_name
			, Scene & p_scene
			, Camera * p_camera
			, bool p_environment
			, SceneNode const * p_ignored
			, SsaoConfig const & p_config );
		/**
		 *\~english
		 *\brief		Constructor for transparent nodes.
		 *\param[in]	p_name			The technique name.
		 *\param[in]	p_scene			The scene for this technique.
		 *\param[in]	p_camera		The camera for this technique (may be null).
		 *\param[in]	p_oit			The OIT status.
		 *\param[in]	p_environment	Pass used for an environment map rendering.
		 *\param[in]	p_ignored		The geometries attached to this node will be ignored in the render.
		 *\param[in]	p_config		The SSAO configuration.
		 *\~french
		 *\brief		Constructeur pour les noeuds transparents.
		 *\param[in]	p_name			Le nom de la technique.
		 *\param[in]	p_scene			La scène pour cette technique.
		 *\param[in]	p_camera		La caméra pour cette technique (peut être nulle).
		 *\param[in]	p_oit			Le statut d'OIT.
		 *\param[in]	p_environment	Passe utilisée pour le rendu d'une texture d'environnement.
		 *\param[in]	p_ignored		Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 *\param[in]	p_config		La configuration du SSAO.
		 */
		C3D_API ForwardRenderTechniquePass( castor::String const & p_name
			, Scene & p_scene
			, Camera * p_camera
			, bool p_oit
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
		C3D_API void render( RenderInfo & p_info, bool p_shadows )override;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::initialiseShadowMaps
		 */
		C3D_API bool initialiseShadowMaps()override;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::cleanupShadowMaps
		 */
		C3D_API void cleanupShadowMaps()override;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::updateShadowMaps
		 */
		C3D_API void updateShadowMaps( RenderQueueArray & p_queues )override;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::renderShadowMaps
		 */
		C3D_API void renderShadowMaps()override;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::addShadowProducer
		 */
		C3D_API void addShadowProducer( Light & p_light )override;

	private:
		/**
		 *\copydoc		castor3d::RenderTechniquePass::doGetDepthMaps
		 */
		C3D_API void doGetDepthMaps( DepthMapArray & p_depthMaps )override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetVertexShaderSource
		 */
		C3D_API GLSL::Shader doGetVertexShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, bool invertNormals )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetLegacyPixelShaderSource
		 */
		C3D_API GLSL::Shader doGetLegacyPixelShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrMRPixelShaderSource
		 */
		C3D_API GLSL::Shader doGetPbrMRPixelShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrSGPixelShaderSource
		 */
		GLSL::Shader doGetPbrSGPixelShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const override;

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
