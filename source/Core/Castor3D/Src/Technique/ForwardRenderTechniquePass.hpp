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
		 *\copydoc		castor3d::RenderTechniquePass::render
		 */
		C3D_API void render( RenderInfo & p_info
			, ShadowMapLightTypeArray & shadowMaps
			, castor::Point2r const & jitter )override;

	private:
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
			, ComparisonFunc alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrMRPixelShaderSource
		 */
		C3D_API glsl::Shader doGetPbrMRPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrSGPixelShaderSource
		 */
		glsl::Shader doGetPbrSGPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const override;
	};
}

#endif
