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
#ifndef ___C3D_POST_EFFECT_H___
#define ___C3D_POST_EFFECT_H___

#include "Texture/TextureUnit.hpp"

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		20/11/2012
	\~english
	\brief		Post render effect base class.
	\remark		A post render effect is an effect applied after 3D rendering and before 2D rendering.
				<br />Post render effects are applied in a cumulative way.
	\~french
	\brief		Classe de base d'effet post rendu.
	\remark		Un effet post rendu est un effet appliqué après le rendu 3D et avant le rendu 2D.
				<br />Les effets post rendu sont appliqués de manière cumulative.
	*/
	class PostEffect
		: public castor::OwnedBy< RenderSystem >
		, public castor::Named
	{
	protected:
		/*!
		\author		Sylvain DOREMUS
		\version	0.9.0
		\date		03/07/2016
		\~english
		\brief		Post render effect surface structure.
		\remark		Holds basic informations for a possible post effect surface: framebuffer and colour texture.
		\~french
		\brief		Surface pour effet post rendu.
		\remark		Contient les informations basiques de surface d'un effet: framebuffer, texture de couleur.
		*/
		struct PostEffectSurface
		{
			/**
			 *\~english
			 *\brief		Constructor.
			 *\param[in]	engine	The engine.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	engine	Le moteur.
			 */
			C3D_API PostEffectSurface( castor3d::Engine & engine );
			/**
			 *\~english
			 *\brief		Initialises the surface.
			 *\param[in]	p_renderTarget	The render target to which is attached this effect.
			 *\param[in]	p_size			The surface size.
			 *\param[in]	p_index			The surface index.
			 *\param[in]	p_sampler		The surface sampler.
			 *\~french
			 *\brief		Initialise la surface.
			 *\param[in]	p_renderTarget	La cible de rendu sur laquelle cet effet s'applique.
			 *\param[in]	p_size			Les dimensions de la surface.
			 *\param[in]	p_index			L'index de la surface.
			 *\param[in]	p_sampler		L'échantillonneur de la surface.
			 */
			C3D_API bool initialise( castor3d::RenderTarget & p_renderTarget, castor::Size const & p_size, uint32_t p_index, castor3d::SamplerSPtr p_sampler );
			/**
			 *\~english
			 *\brief		Cleans up the surface.
			 *\~french
			 *\brief		Nettoie la surface.
			 */
			C3D_API void cleanup();

			//!\~english	The surface framebuffer.
			//!\~french		Le framebuffer de la surface.
			castor3d::FrameBufferSPtr m_fbo;
			//!\~english	The surface colour texture.
			//!\~french		La texture couleur de la surface.
			castor3d::TextureUnit m_colourTexture;
			//!\~english	The attach between framebuffer and texture.
			//!\~french		L'attache entre la texture et le framebuffer.
			castor3d::TextureAttachmentSPtr m_colourAttach;
			//!\~english	The surface dimensions.
			//!\~french		Les dimensions de la surface.
			castor::Size m_size;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_name			The effect name.
		 *\param[in]	p_renderTarget	The render target to which is attached this effect.
		 *\param[in]	renderSystem	The render system.
		 *\param[in]	p_params		The optional parameters.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_name			Le nom de l'effet.
		 *\param[in]	p_renderTarget	La cible de rendu sur laquelle cet effet s'applique.
		 *\param[in]	renderSystem	Le render system.
		 *\param[in]	p_params		Les paramètres optionnels.
		 */
		C3D_API PostEffect( castor::String const & p_name, RenderTarget & p_renderTarget, RenderSystem & renderSystem, Parameters const & p_params );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~PostEffect();
		/**
		 *\~english
		 *\brief		Writes the effect into a text file.
		 *\param[in]	p_file	The file.
		 *\~french
		 *\brief		Ecrit l'effet dans un fichier texte.
		 *\param[in]	p_file	Le fichier.
		 */
		C3D_API bool writeInto( castor::TextFile & p_file );
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\return		\p true if ok.
		 */
		C3D_API virtual bool initialise() = 0;
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API virtual void cleanup() = 0;
		/**
		 *\~english
		 *\brief			Render function, applies the effect to the given framebuffer.
		 *\param[in,out]	p_framebuffer	The framebuffer.
		 *\return			\p true if ok.
		 *\~french
		 *\brief			Fonction de rendu, applique l'effet au tampon d'image donné.
		 *\param[in,out]	p_framebuffer	Le tampon d'image.
		 *\return			\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool apply( FrameBuffer & p_framebuffer ) = 0;

	private:
		/**
		 *\~english
		 *\brief		Writes the effect into a text file.
		 *\param[in]	p_file	The file.
		 *\~french
		 *\brief		Ecrit l'effet dans un fichier texte.
		 *\param[in]	p_file	Le fichier.
		 */
		C3D_API virtual bool doWriteInto( castor::TextFile & p_file ) = 0;

	protected:
		//!\~english	The render target to which this effect is attached.
		//!\~french		La cible de rendu à laquelle est attachée cet effet.
		RenderTarget & m_renderTarget;
	};
}

#endif
