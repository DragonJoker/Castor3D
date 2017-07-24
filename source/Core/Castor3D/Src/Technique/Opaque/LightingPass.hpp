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
#ifndef ___C3D_DeferredLightingPass_H___
#define ___C3D_DeferredLightingPass_H___

#include "CombinePass.hpp"
#include "ReflectionPass.hpp"

namespace Castor3D
{
	class OpaquePass;
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Handles the light passes.
	\~french
	\brief		Gère les passes d'éclairage.
	*/
	class LightingPass
	{
	private:
		using LightPasses = std::array< std::unique_ptr< LightPass >, size_t( LightType::eCount ) >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	size		The render area dimensions.
		 *\param[in]	scene		The scene.
		 *\param[in]	opaque		The opaque pass.
		 *\param[in]	depthAttach	The depth buffer attach.
		 *\param[in]	sceneUbo	The scene UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	size		Les dimensions de la zone de rendu.
		 *\param[in]	scene		La scène.
		 *\param[in]	opaque		La passe opaque.
		 *\param[in]	depthAttach	L'attache du tampon de profondeur.
		 *\param[in]	sceneUbo	L'UBO de scène.
		 */
		LightingPass( Engine & engine
			, Castor::Size const & size
			, Scene const & scene
			, OpaquePass & opaque
			, FrameBufferAttachment & depthAttach
			, SceneUbo & sceneUbo );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~LightingPass();
		/**
		 *\~english
		 *\brief		Renders the light passes on currently bound framebuffer.
		 *\param[in]	scene		The scene.
		 *\param[in]	camera		The viewing camera.
		 *\param[in]	gp			The geometry pass result.
		 *\param[in]	invViewProj	The inverse view projection matrix.
		 *\param[in]	invView		The inverse view matrix.
		 *\param[in]	invProj		The inverse projection matrix.
		 *\~french
		 *\brief		Dessine les passes d'éclairage sur le tampon d'image donné.
		 *\param[in]	scene		La scène.
		 *\param[in]	camera		La caméra.
		 *\param[in]	gp			Le résultat de la geometry pass.
		 *\param[in]	invViewProj	La matrice vue projection inversée.
		 *\param[in]	invView		La matrice vue inversée.
		 *\param[in]	invProj		La matrice projection inversée.
		 */
		bool Render( Scene const & scene
			, Camera const & camera
			, GeometryPassResult const & gp
			, Castor::Matrix4x4r const & invViewProj
			, Castor::Matrix4x4r const & invView
			, Castor::Matrix4x4r const & invProj
			, RenderInfo & info );
		/**
		 *\~english
		 *\return		The light pass result.
		 *\~french
		 *\return		Le résultat de la passe d'éclairage.
		 */
		inline TextureUnit const & GetResult()const
		{
			return m_result;
		}

	private:
		/**
		 *\~english
		 *\brief		Renders the light passes for a light sources type on currently bound framebuffer.
		 *\param[in]		scene		The scene.
		 *\param[in]		camera		The viewing camera.
		 *\param[in]		type		The light sources type.
		 *\param[in]		gp			The geometry pass result.
		 *\param[in]		invViewProj	The inverse view projection matrix.
		 *\param[in]		invView		The inverse view matrix.
		 *\param[in]		invProj		The inverse projection matrix.
		 *\param[in]		first		Tells if this is the first light pass (\p true) or not (\p false).
		 *\~french
		 *\brief			Dessine les passes d'éclairage d'un type de source sur le tampon d'image donné.
		 *\param[in]		scene		La scène.
		 *\param[in]		camera		La caméra.
		 *\param[in]		type		Le type de sources lumineuse.
		 *\param[in]		gp			Le résultat de la geometry pass.
		 *\param[in]		invViewProj	La matrice vue projection inversée.
		 *\param[in]		invView		La matrice vue inversée.
		 *\param[in]		invProj		La matrice projection inversée.
		 *\param[in,out]	first		Dit si cette passe d'éclairage est la première (\p true) ou pas (\p false).
		 */
		void DoRenderLights( Scene const & scene
			, Camera const & camera
			, LightType type
			, GeometryPassResult const & gp
			, Castor::Matrix4x4r const & invViewProj
			, Castor::Matrix4x4r const & invView
			, Castor::Matrix4x4r const & invProj
			, bool & first );

	private:
		Castor::Size const m_size;
		//!\~english	The light pass output.
		//!\~french		La sortie de la passe de lumières.
		TextureUnit m_result;
		//!\~english	The target FBO.
		//!\~french		Le FBO cible.
		FrameBufferSPtr m_frameBuffer;
		//!\~english	The attachments between textures and deferred shading frame buffer.
		//!\~french		Les attaches entre les textures et le tampon deferred shading.
		TextureAttachmentSPtr m_resultAttach;
		//!\~english	The light passes.
		//!\~french		Les passes de lumière.
		LightPasses m_lightPass;
		//!\~english	The light passes, with shadows.
		//!\~french		Les passes de lumière, avec ombres.
		LightPasses m_lightPassShadow;
		//!\~english	The render pass timer.
		//!\~french		Le timer de la passe de rendu.
		RenderPassTimerSPtr m_timer;
	};
}

#endif
