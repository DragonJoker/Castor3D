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
#ifndef ___C3D_DeferredOpaquePass_H___
#define ___C3D_DeferredOpaquePass_H___

#include <Technique/RenderTechniquePass.hpp>

#include "ShadowMap/DeferredShadowMapDirectional.hpp"
#include "ShadowMap/DeferredShadowMapPoint.hpp"
#include "ShadowMap/DeferredShadowMapSpot.hpp"

namespace Castor3D
{
	class RenderTechnique;
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Deferred lighting Render technique pass.
	\~french
	\brief		Classe de passe de technique de rendu implémentant le Deferred lighting.
	*/
	class OpaquePass
		: public RenderTechniquePass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	scene	The scene for this technique.
		 *\param[in]	camera	The camera for this technique (may be null).
		 *\param[in]	config	The SSAO configuration.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	scene	La scène pour cette technique.
		 *\param[in]	camera	La caméra pour cette technique (peut être nulle).
		 *\param[in]	config	La configuration du SSAO.
		 */
		OpaquePass( Scene & scene
			, Camera * camera
			, SsaoConfig const & config );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~OpaquePass();
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
		void Render( RenderInfo & info, bool shadows )override;
		/**
		 *\copydoc		Castor3D::RenderPass::InitialiseShadowMaps
		 */
		bool InitialiseShadowMaps()override;
		/**
		 *\copydoc		Castor3D::RenderPass::CleanupShadowMaps
		 */
		void CleanupShadowMaps()override;
		/**
		 *\copydoc		Castor3D::RenderPass::UpdateShadowMaps
		 */
		void UpdateShadowMaps( RenderQueueArray & queues )override;
		/**
		 *\copydoc		Castor3D::RenderPass::RenderShadowMaps
		 */
		void RenderShadowMaps()override;
		/**
		 *\copydoc		Castor3D::RenderTechniquePass::AddShadowProducer
		 */
		C3D_API void AddShadowProducer( Light & light )override;
		/**
		 *\~english
		 *\return		The directional light shadow map.
		 *\~french
		 *\return		La map d'ombre de la lumière directionnelle.
		 */
		inline DeferredShadowMapDirectional & GetDirectionalShadowMap()
		{
			return m_directionalShadowMap;
		}
		/**
		 *\~english
		 *\return		The spot lights shadow map.
		 *\~french
		 *\return		La map d'ombre des lumières projecteur.
		 */
		inline DeferredShadowMapSpot & GetSpotShadowMap()
		{
			return m_spotShadowMap;
		}
		/**
		 *\~english
		 *\return		The point lights shadow map.
		 *\~french
		 *\return		La map d'ombre des lumières ponctuelles.
		 */
		inline DeferredShadowMapPoint & GetPointShadowMaps()
		{
			return m_pointShadowMap;
		}

	protected:
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetDepthMaps
		 */
		void DoGetDepthMaps( DepthMapArray & depthMaps )override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoUpdateFlags
		 */
		void DoUpdateFlags( TextureChannels & textureFlags
			, ProgramFlags & programFlags
			, SceneFlags & sceneFlags )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetVertexShaderSource
		 */
		GLSL::Shader DoGetVertexShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, bool invertNormals )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetLegacyPixelShaderSource
		 */
		GLSL::Shader DoGetLegacyPixelShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetPbrMRPixelShaderSource
		 */
		GLSL::Shader DoGetPbrMRPixelShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetPbrSGPixelShaderSource
		 */
		GLSL::Shader DoGetPbrSGPixelShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoUpdatePipeline
		 */
		void DoUpdatePipeline( RenderPipeline & pipeline )const override;

	private:
		//!\~english	The shadow map used for directional lights.
		//!\~french		Le mappage d'ombres utilisée pour les lumières de type directionnelles.
		DeferredShadowMapDirectional m_directionalShadowMap;
		//!\~english	The shadow map used for spot lights.
		//!\~french		Le mappage d'ombres utilisée pour les lumières de type spot.
		DeferredShadowMapSpot m_spotShadowMap;
		//!\~english	The shadow map used for pont lights.
		//!\~french		Le mappage d'ombres utilisée pour les lumières de type point.
		DeferredShadowMapPoint m_pointShadowMap;

	private:
		static Castor::String const Output1;
		static Castor::String const Output2;
		static Castor::String const Output3;
		static Castor::String const Output4;
	};
}

#endif
