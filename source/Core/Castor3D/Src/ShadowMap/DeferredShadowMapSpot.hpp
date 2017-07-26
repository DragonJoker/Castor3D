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
#ifndef ___C3D_DeferredfShadowMapSpot_H___
#define ___C3D_DeferredfShadowMapSpot_H___

#include <ShadowMap/ShadowMap.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		30/08/2016
	\~english
	\brief		Shadow mapping implementation for spot lights.
	\~french
	\brief		Implémentation du mappage d'ombres pour les lumières spot.
	*/
	class DeferredShadowMapSpot
		: public ShadowMap
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		DeferredShadowMapSpot( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~DeferredShadowMapSpot();
		/**
		 *\~english
		 *\brief		Updates the passes, selecting the lights that will project shadows.
		 *\remarks		Gather the render queues, for further update.
		 *\param[in]	p_camera	The viewer camera.
		 *\param[out]	p_queues	Receives the render queues needed for the rendering of the frame.
		 *\~french
		 *\brief		Met à jour les passes, en sélectionnant les lumières qui projetteront une ombre.
		 *\remarks		Récupère les files de rendu, pour mise à jour ultérieure.
		 *\param[in]	p_camera	La caméra de l'observateur.
		 *\param[out]	p_queues	Reçoit les files de rendu nécessaires pour le dessin de la frame.
		 */
		void Update( Camera const & p_camera
			, RenderQueueArray & p_queues );
		/**
		 *\~english
		 *\brief		Renders the given light's shadow map.
		 *\~french
		 *\brief		Dessine la shadow map de la lumière donnée.
		 */
		void Render( SpotLight const & p_light );
		/**
		 *\~english
		 *\return		The shadow map.
		 *\~english
		 *\return		La map d'ombres.
		 */
		inline TextureUnit & GetTexture()
		{
			return m_shadowMap;
		}
		/**
		 *\~english
		 *\return		The shadow map.
		 *\~english
		 *\return		La map d'ombres.
		 */
		inline TextureUnit const & GetTexture()const
		{
			return m_shadowMap;
		}

	private:
		/**
		 *\copydoc		Castor3D::ShadowMap::DoGetMaxPasses
		 */
		int32_t DoGetMaxPasses()const override;
		/**
		 *\copydoc		Castor3D::ShadowMap::DoGetSize
		 */
		Castor::Size DoGetSize()const override;
		/**
		 *\copydoc		Castor3D::ShadowMap::DoInitialise
		 */
		void DoInitialise()override;
		/**
		 *\copydoc		Castor3D::ShadowMap::DoCleanup
		 */
		void DoCleanup()override;
		/**
		 *\copydoc		Castor3D::ShadowMap::DoCreatePass
		 */
		ShadowMapPassSPtr DoCreatePass( Light & p_light )const override;
		/**
		 *\copydoc		Castor3D::ShadowMap::DoUpdateFlags
		 */
		void DoUpdateFlags( TextureChannels & textureFlags
			, ProgramFlags & programFlags
			, SceneFlags & sceneFlags )const override;
		/**
		 *\copydoc		Castor3D::ShadowMap::DoGetPixelShaderSource
		 */
		GLSL::Shader DoGetPixelShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const override;

	private:
		//!\~english	The attach between depth buffer and main frame buffer.
		//!\~french		L'attache entre le tampon profondeur et le tampon principal.
		TextureAttachmentSPtr m_depthAttach;
		//!\~english	The shadow map texture.
		//!\~french		La texture de mappage d'ombres.
		TextureUnit m_shadowMap;
	};
}

#endif
