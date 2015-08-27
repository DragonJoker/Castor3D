/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___C3D_TARGET_RENDERER_H___
#define ___C3D_TARGET_RENDERER_H___

#include "Castor3DPrerequisites.hpp"
#include "Renderer.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date 		08/11/2012
	\~english
	\brief		The renderer for RenderTarget
	\~french
	\brief		Le renderer pour une RenderTarget
	\remark
	*/
	class C3D_API TargetRenderer
		:	public Renderer< RenderTarget, TargetRenderer >
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\remark		Only RenderSystem can use it
		 *\param[in]	p_pRenderSystem	The RenderSystem
		 *\~french
		 *\brief		Constructeur
		 *\remark		Seul RenderSystem peut l'utiliser
		 *\param[in]	p_pRenderSystem	Le RenderSystem
		 */
		TargetRenderer( RenderSystem * p_pRenderSystem );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~TargetRenderer();
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\remark		Initialises the frame buffer
		 *\return		\p false if any problem occured
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\remark		Initialise le tampon d'image
		 *\return		\p false si un problème a été rencontré
		 */
		virtual bool Initialise() = 0;
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		virtual void Cleanup() = 0;
		/**
		 *\~english
		 *\brief		Function to start drawing a scene
		 *\~french
		 *\brief		Fonction pour débuter le dessin d'une scène
		 */
		virtual void BeginScene() = 0;
		/**
		 *\~english
		 *\brief		Function to end drawing a scene
		 *\~french
		 *\brief		Fonction pour terminer le dessin d'une scène
		 */
		virtual void EndScene() = 0;
		/**
		 *\~english
		 *\brief		Creates an attachment to a render buffer
		 *\param[in]	p_pRenderBuffer	The render buffer
		 *\return		The created attachment
		 *\~french
		 *\brief		Crée une attache à un tampon de rendu
		 *\param[in]	p_pRenderBuffer	Le tampon de rendu
		 *\return		L'attache créée
		 */
		virtual RenderBufferAttachmentSPtr CreateAttachment( RenderBufferSPtr p_pRenderBuffer )const = 0;
		/**
		 *\~english
		 *\brief		Creates an attachment to a texture
		 *\param[in]	p_pTexture	The texture
		 *\return		The created attachment
		 *\~french
		 *\brief		Crée une attache à une texture
		 *\param[in]	p_pTexture	La texture
		 *\return		L'attache créée
		 */
		virtual TextureAttachmentSPtr CreateAttachment( DynamicTextureSPtr p_pTexture )const = 0;
		/**
		 *\~english
		 *\brief		Creates a frame buffer
		 *\return		The created frame buffer
		 *\~french
		 *\brief		Crée un tampon d'image
		 *\return		Le tampon d'image créé
		 */
		virtual FrameBufferSPtr CreateFrameBuffer()const = 0;
	};
}

#pragma warning( pop )

#endif
