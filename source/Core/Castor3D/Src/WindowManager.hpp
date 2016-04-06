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
#ifndef ___C3D_WINDOW_MANAGER_H___
#define ___C3D_WINDOW_MANAGER_H___

#include "ResourceManager.hpp"
#include "RenderWindow.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to get an object type name.
	\~french
	\brief		Structure permettant de récupérer le nom du type d'un objet.
	*/
	template<> struct ManagedObjectNamer< RenderWindow >
	{
		C3D_API static const Castor::String Name;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Render windows manager.
	\~french
	\brief		Gestionnaire de fenêtres de rendu.
	*/
	class WindowManager
		: private ResourceManager< Castor::String, RenderWindow >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API WindowManager( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~WindowManager();
		/**
		 *\~english
		 *\brief		Creates an object.
		 *\param[in]	p_name		The object name.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un objet.
		 *\param[in]	p_name		Le nom de l'objet.
		 *\return		L'objet créé.
		 */
		C3D_API RenderWindowSPtr Create( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Renders one frame for each window.
		 *\param[in]	p_force		Forces the rendering.
		 *\~english
		 *\brief		Dessine une image de chaque fenêtre.
		 *\param[in]	p_force		Dit si on force le rendu.
		 */
		C3D_API void Render( bool p_force );

	public:
		using ResourceManager< Castor::String, RenderWindow >::lock;
		using ResourceManager< Castor::String, RenderWindow >::unlock;
		using ResourceManager< Castor::String, RenderWindow >::begin;
		using ResourceManager< Castor::String, RenderWindow >::end;
		using ResourceManager< Castor::String, RenderWindow >::Has;
		using ResourceManager< Castor::String, RenderWindow >::Find;
		using ResourceManager< Castor::String, RenderWindow >::Insert;
		using ResourceManager< Castor::String, RenderWindow >::Remove;
		using ResourceManager< Castor::String, RenderWindow >::Cleanup;
		using ResourceManager< Castor::String, RenderWindow >::Clear;
		using ResourceManager< Castor::String, RenderWindow >::GetEngine;
		using ResourceManager< Castor::String, RenderWindow >::SetRenderSystem;
	};
}

#endif
