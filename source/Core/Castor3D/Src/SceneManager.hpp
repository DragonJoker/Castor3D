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
#ifndef ___C3D_SCENE_MANAGER_H___
#define ___C3D_SCENE_MANAGER_H___

#include "ResourceManager.hpp"

#include "LightFactory.hpp"
#include "Scene.hpp"

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
	template<> struct ManagedObjectNamer< Scene >
	{
		C3D_API static const Castor::String Name;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Scene manager.
	\~french
	\brief		Gestionnaire de Scene.
	*/
	class SceneManager
		: public ResourceManager< Castor::String, Scene >
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
		C3D_API SceneManager( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~SceneManager();
		/**
		 *\~english
		 *\brief		Updates all scenes.
		 *\~french
		 *\brief		Met à jout toutes les scènes.
		 */
		C3D_API void Update();
		/**
		 *\~english
		 *\brief		Retrieves the Light factory.
		 *\return		The factory.
		 *\~french
		 *\brief		Récupère la fabrique de Light.
		 *\return		La fabrique.
		 */
		inline LightFactory const & GetFactory()const
		{
			return m_factory;
		}
		/**
		 *\~english
		 *\brief		Retrieves the Light factory.
		 *\return		The factory.
		 *\~french
		 *\brief		Récupère la fabrique de Light.
		 *\return		La fabrique.
		 */
		inline LightFactory & GetFactory()
		{
			return m_factory;
		}

	private:
		//!\~english The LightCategory factory	\~french La fabrique de LightCategory
		LightFactory m_factory;
	};
}

#endif
