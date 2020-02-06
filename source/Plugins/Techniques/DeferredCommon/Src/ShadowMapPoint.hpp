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
#ifndef ___C3D_DeferredShadowMapPoint_H___
#define ___C3D_DeferredShadowMapPoint_H___

#include <ShadowMap/ShadowMap.hpp>
#include <Render/Viewport.hpp>

namespace deferred_common
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		30/08/2016
	\~english
	\brief		Shadow mapping implementation.
	\~french
	\brief		Implémentation du mappage d'ombres.
	*/
	class ShadowMapPoint
		: public Castor3D::ShadowMap
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\param[in]	p_textures	The textures.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 *\param[in]	p_textures	Les textures.
		 */
		ShadowMapPoint( Castor3D::Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~ShadowMapPoint();
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
		void Update( Castor3D::Camera const & p_camera
			, Castor3D::RenderQueueArray & p_queues );
		/**
		 *\~english
		 *\brief		Renders the given light's shadow map.
		 *\~french
		 *\brief		Dessine la shadow map de la lumière donnée.
		 */
		void Render( Castor3D::PointLight const & p_light );
		/**
		 *\~english
		 *\return		The shadow map.
		 *\~english
		 *\return		La map d'ombres.
		 */
		inline Castor3D::TextureUnit & GetTexture()
		{
			return m_shadowMap;
		}
		/**
		 *\~english
		 *\return		The shadow map.
		 *\~english
		 *\return		La map d'ombres.
		 */
		inline Castor3D::TextureUnit const & GetTexture()const
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
		Castor3D::ShadowMapPassSPtr DoCreatePass( Castor3D::Light & p_light )const override;
		/**
		 *\copydoc		Castor3D::ShadowMap::DoUpdateFlags
		 */
		void DoUpdateFlags( Castor3D::TextureChannels & p_textureFlags
			, Castor3D::ProgramFlags & p_programFlags
			, Castor3D::SceneFlags & p_sceneFlags )const override;
		/**
		 *\copydoc		Castor3D::ShadowMap::DoGetVertexShaderSource
		 */
		Castor::String DoGetVertexShaderSource( Castor3D::TextureChannels const & p_textureFlags
			, Castor3D::ProgramFlags const & p_programFlags
			, Castor3D::SceneFlags const & p_sceneFlags
			, bool p_invertNormals )const override;
		/**
		 *\copydoc		Castor3D::ShadowMap::DoGetGeometryShaderSource
		 */
		Castor::String DoGetGeometryShaderSource( Castor3D::TextureChannels const & p_textureFlags
			, Castor3D::ProgramFlags const & p_programFlags
			, Castor3D::SceneFlags const & p_sceneFlags )const override;
		/**
		 *\copydoc		Castor3D::ShadowMap::DoGetPixelShaderSource
		 */
		Castor::String DoGetPixelShaderSource( Castor3D::TextureChannels const & p_textureFlags
			, Castor3D::ProgramFlags const & p_programFlags
			, Castor3D::SceneFlags const & p_sceneFlags )const override;

	private:
		using CubeColourAttachment = std::array< Castor3D::TextureAttachmentSPtr, size_t( Castor3D::CubeMapFace::eCount ) >;
		using CubeDepthAttachment = Castor3D::RenderBufferAttachmentSPtr;
		//!\~english	The attach between depth buffer and main frame buffer.
		//!\~french		L'attache entre le tampon de profondeur et le tampon principal.
		CubeDepthAttachment m_depthAttach;
		//!\~english	The depth buffer.
		//!\~french		Le tampon de profondeur.
		Castor3D::DepthStencilRenderBufferSPtr m_depthBuffer;
		//!\~english	The attach between colour buffer and main frame buffer.
		//!\~french		L'attache entre le tampon de couleur et le tampon principal.
		CubeColourAttachment m_colourAttach;
		//!\~english	The shadow map texture.
		//!\~french		La texture de mappage d'ombres.
		Castor3D::TextureUnit m_shadowMap;
	};
}

#endif
