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
#ifndef ___C3D_RESOURCE_MANAGER_H___
#define ___C3D_RESOURCE_MANAGER_H___

#include "Manager.hpp"

#include "Scene/Scene.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to retrieve an ObjectManager Engine instance.
	\~french
	\brief		Structure permettant de récupérer l'Engine d'un ObjectManager.
	*/
	template< typename Owner >
	struct ResourceManagerEngineGetter;
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to retrieve an ObjectManager Engine instance.
	\~french
	\brief		Structure permettant de récupérer l'Engine d'un ObjectManager.
	*/
	template<>
	struct ResourceManagerEngineGetter< Engine >
	{
		template< typename Key, typename Elem >
		static Engine * Get( Manager< Key, Elem, Engine, ResourceManagerEngineGetter< Engine > > const & p_this )
		{
			return static_cast< Castor::OwnedBy< Engine > const & >( p_this ).GetEngine();
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to retrieve an ObjectManager Engine instance.
	\~french
	\brief		Structure permettant de récupérer l'Engine d'un ObjectManager.
	*/
	template<>
	struct ResourceManagerEngineGetter< Scene >
	{
		template< typename Key, typename Elem >
		static Engine * Get( Manager< Key, Elem, Scene, ResourceManagerEngineGetter< Scene > > const & p_this )
		{
			return static_cast< Castor::OwnedBy< Scene > const & >( p_this ).GetScene()->GetEngine();
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Base class for an element manager.
	\~french
	\brief		Classe de base pour un gestionnaire d'éléments.
	*/
	template< typename Key, typename Elem, typename Owner=Engine >
	class ResourceManager
		: public Manager< Key, Elem, Owner, ResourceManagerEngineGetter< Owner > >
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_owner	The owner.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_owner	Le propriétaire.
		 */
		inline ResourceManager( Owner & p_owner )
			: Manager< Key, Elem, Owner, ResourceManagerEngineGetter< Owner > >( p_owner )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~ResourceManager()
		{
		}
	};
}

#endif
