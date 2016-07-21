/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_POST_EFFECT_H___
#define ___C3D_POST_EFFECT_H___

#include "Texture/TextureUnit.hpp"

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>

namespace Castor3D
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
		: public Castor::OwnedBy< RenderSystem >
		, public Castor::Named
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
			 *\param[in]	p_engine	The engine.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	p_engine	Le moteur.
			 */
			C3D_API PostEffectSurface( Castor3D::Engine & p_engine );
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
			C3D_API bool Initialise( Castor3D::RenderTarget & p_renderTarget, Castor::Size const & p_size, uint32_t p_index, Castor3D::SamplerSPtr p_sampler );
			/**
			 *\~english
			 *\brief		Cleans up the surface.
			 *\~french
			 *\brief		Nettoie la surface.
			 */
			C3D_API void Cleanup();

			//!\~english	The surface framebuffer.
			//!\~french		Le framebuffer de la surface.
			Castor3D::FrameBufferSPtr m_fbo;
			//!\~english	The surface colour texture.
			//!\~french		La texture couleur de la surface.
			Castor3D::TextureUnit m_colourTexture;
			//!\~english	The attach between framebuffer and texture.
			//!\~french		L'attache entre la texture et le framebuffer.
			Castor3D::TextureAttachmentSPtr m_colourAttach;
			//!\~english	The surface dimensions.
			//!\~french		Les dimensions de la surface.
			Castor::Size m_size;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_name			The effect name.
		 *\param[in]	p_renderTarget	The render target to which is attached this effect.
		 *\param[in]	p_renderSystem	The render system.
		 *\param[in]	p_params		The optional parameters.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_name			Le nom de l'effet.
		 *\param[in]	p_renderTarget	La cible de rendu sur laquelle cet effet s'applique.
		 *\param[in]	p_renderSystem	Le render system.
		 *\param[in]	p_params		Les paramètres optionnels.
		 */
		C3D_API PostEffect( Castor::String const & p_name, RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params );
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
		C3D_API bool WriteInto( Castor::TextFile & p_file );
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\return		\p true if ok.
		 */
		C3D_API virtual bool Initialise() = 0;
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API virtual void Cleanup() = 0;
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
		C3D_API virtual bool Apply( FrameBuffer & p_framebuffer ) = 0;

	private:
		/**
		 *\~english
		 *\brief		Writes the effect into a text file.
		 *\param[in]	p_file	The file.
		 *\~french
		 *\brief		Ecrit l'effet dans un fichier texte.
		 *\param[in]	p_file	Le fichier.
		 */
		C3D_API virtual bool DoWriteInto( Castor::TextFile & p_file ) = 0;

	protected:
		//!\~english	The render target to which this effect is attached.
		//!\~french		La cible de rendu à laquelle est attachée cet effet.
		RenderTarget & m_renderTarget;
	};
}

#endif
