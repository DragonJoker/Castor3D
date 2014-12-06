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
#ifndef ___C3D_WINDOW_RENDERER_H___
#define ___C3D_WINDOW_RENDERER_H___

#include "RenderWindow.hpp"
#include "Renderer.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		A window renderer
	\remark		Creates the window context, renders the window, resizes the window
	\~french
	\brief		Un renderer de fenêtre
	\remark		Crée le contexte de rendu, rend la fenêtre, redimensionne la fenêtre
	*/
	class C3D_API WindowRenderer
		:	public Renderer< RenderWindow, WindowRenderer >
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\remark		Only RenderSystem can use it
		 *\~french
		 *\brief		Constructeur
		 *\remark		Seul RenderSystem peut l'utiliser
		 */
		WindowRenderer( RenderSystem * p_pRenderSystem );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~WindowRenderer();
		/**
		 *\~english
		 *\brief		Initialises the context
		 *\~french
		 *\brief		Initialise le contexte
		 */
		virtual bool Initialise();
		/**
		 *\~english
		 *\brief		Cleans up the instance
		 *\~french
		 *\brief		Nettoie l'instance
		 */
		virtual void Cleanup();
		/**
		 *\~english
		 *\brief		Starts the scene render : flushes the window, ...
		 *\~french
		 *\brief		Débute le rendu de la scène : vide la fenêtre, ...
		 */
		virtual bool BeginScene() = 0;
		/**
		 *\~english
		 *\brief		Function to end scene render
		 *\return		\p false if the render target doesn't supports frame buffer
		 *\~french
		 *\brief		Fonction pour terminer le rendu d'une scène
		 *\return		\p false si la cible du rendu ne supporte pas les frame buffers
		 */
		virtual bool EndScene() = 0;
		/**
		 *\~english
		 *\brief		Ends the render : swaps buffers ,...
		 *\~french
		 *\brief		Termine le rendu : échange les buffers, ...
		 */
		virtual void EndRender() = 0;
		/**
		 *\~english
		 *\brief		Activates the window context
		 *\~french
		 *\brief		Active le contexte de la fenêtre
		 */
		virtual bool SetCurrent() = 0;
		/**
		 *\~english
		 *\brief		Deactivates the window context
		 *\~french
		 *\brief		Désctive le contexte de la fenêtre
		 */
		virtual void EndCurrent() = 0;

	protected:
		/**
		 *\~english
		 *\brief		Initialises the context
		 *\~french
		 *\brief		Initialise le contexte
		 */
		virtual bool DoInitialise() = 0;
		/**
		 *\~english
		 *\brief		Cleans up the instance
		 *\~french
		 *\brief		Nettoie l'instance
		 */
		virtual void DoCleanup() = 0;
	};
}

#pragma warning( pop )

#endif
