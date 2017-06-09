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
		 *\param[in]	p_engine		The engine.
		 *\param[in]	p_size			The render area dimensions.
		 *\param[in]	p_scene			The scene.
		 *\param[in]	p_opaque		The opaque pass.
		 *\param[in]	p_depthAttach	The depth buffer attach.
		 *\param[in]	p_sceneUbo		The scene UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine		Le moteur.
		 *\param[in]	p_size			Les dimensions de la zone de rendu.
		 *\param[in]	p_scene			La scène.
		 *\param[in]	p_opaque		La passe opaque.
		 *\param[in]	p_depthAttach	L'attache du tampon de profondeur.
		 *\param[in]	p_sceneUbo		L'UBO de scène.
		 */
		LightingPass( Engine & p_engine
			, Castor::Size const & p_size
			, Scene const & p_scene
			, OpaquePass & p_opaque
			, FrameBufferAttachment & p_depthAttach
			, SceneUbo & p_sceneUbo );
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
		 *\param[in]	p_scene			The scene.
		 *\param[in]	p_camera		The viewing camera.
		 *\param[in]	p_gp			The geometry pass result.
		 *\param[in]	p_invViewProj	The inverse view projection matrix.
		 *\param[in]	p_invView		The inverse view matrix.
		 *\param[in]	p_invProj		The inverse projection matrix.
		 *\~french
		 *\brief		Dessine les passes d'éclairage sur le tampon d'image donné.
		 *\param[in]	p_scene			La scène.
		 *\param[in]	p_camera		La caméra.
		 *\param[in]	p_gp			Le résultat de la geometry pass.
		 *\param[in]	p_invViewProj	La matrice vue projection inversée.
		 *\param[in]	p_invView		La matrice vue inversée.
		 *\param[in]	p_invProj		La matrice projection inversée.
		 */
		bool Render( Scene const & p_scene
			, Camera const & p_camera
			, GeometryPassResult const & p_gp
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj );
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
		 *\param[in]		p_scene			The scene.
		 *\param[in]		p_camera		The viewing camera.
		 *\param[in]		p_type			The light sources type.
		 *\param[in]		p_gp			The geometry pass result.
		 *\param[in]		p_invViewProj	The inverse view projection matrix.
		 *\param[in]		p_invView		The inverse view matrix.
		 *\param[in]		p_invProj		The inverse projection matrix.
		 *\param[in]		p_first			Tells if this is the first light pass (\p true) or not (\p false).
		 *\~french
		 *\brief			Dessine les passes d'éclairage d'un type de source sur le tampon d'image donné.
		 *\param[in]		p_scene			La scène.
		 *\param[in]		p_camera		La caméra.
		 *\param[in]		p_type			Le type de sources lumineuse.
		 *\param[in]		p_gp			Le résultat de la geometry pass.
		 *\param[in]		p_invViewProj	La matrice vue projection inversée.
		 *\param[in]		p_invView		La matrice vue inversée.
		 *\param[in]		p_invProj		La matrice projection inversée.
		 *\param[in,out]	p_first			Dit si cette passe d'éclairage est la première (\p true) ou pas (\p false).
		 */
		void DoRenderLights( Scene const & p_scene
			, Camera const & p_camera
			, LightType p_type
			, GeometryPassResult const & p_gp
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj
			, bool & p_first );

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
	};
}

#endif
