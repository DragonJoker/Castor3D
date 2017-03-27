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

#include "ShadowMapDirectional.hpp"
#include "ShadowMapPoint.hpp"
#include "ShadowMapSpot.hpp"

namespace deferred_common
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
		: public Castor3D::RenderTechniquePass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_renderTarget	The render target for this technique.
		 *\param[in]	p_technique		The parent render technique.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_renderTarget	La render target pour cette technique.
		 *\param[in]	p_technique		La technique de rendu parente.
		 */
		OpaquePass( Castor3D::RenderTarget & p_renderTarget
			, Castor3D::RenderTechnique & p_technique );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~OpaquePass();
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
		void UpdateShadowMaps( Castor3D::RenderQueueArray & p_queues )override;
		/**
		 *\copydoc		Castor3D::RenderPass::RenderShadowMaps
		 */
		void RenderShadowMaps()override;
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

	protected:
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetDepthMaps
		 */
		void DoGetDepthMaps( Castor3D::DepthMapArray & p_depthMaps )override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoUpdateFlags
		 */
		void DoUpdateFlags( Castor3D::TextureChannels & p_textureFlags
			, Castor3D::ProgramFlags & p_programFlags
			, Castor3D::SceneFlags & p_sceneFlags )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetPixelShaderSource
		 */
		Castor::String DoGetPixelShaderSource( Castor3D::TextureChannels const & p_textureFlags
			, Castor3D::ProgramFlags const & p_programFlags
			, Castor3D::SceneFlags const & p_sceneFlags )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoUpdatePipeline
		 */
		void DoUpdatePipeline( Castor3D::RenderPipeline & p_pipeline )const override;

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
