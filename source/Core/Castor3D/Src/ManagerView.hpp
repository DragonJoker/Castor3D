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
#ifndef ___C3D_MANAGER_VIEW_H___
#define ___C3D_MANAGER_VIEW_H___

#include "ResourceManager.hpp"

#include <Named.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		14/03/2016
	\version	0.8.0
	\~english
	\brief		View on a resource manager.
	\remarks	Allows deletion of elements created through the view, and only those.
	\~french
	\brief		Vue sur un gestionnaire de ressources.
	\remarks	Permet de supprimer tous les éléments créés via la vue et uniquement ceux là.
	*/
	template< typename ResourceType, typename ManagerType, eEVENT_TYPE EventType >
	class ManagerView
		: public Castor::Named
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_name		The view name.
		 *\param[in]	p_manager	The viewed manager.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name		Le nom de la vue.
		 *\param[in]	p_manager	Le gestionnaire vu.
		 */
		inline ManagerView( Castor::String const & p_name, ManagerType & p_manager );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		inline ~ManagerView();
		/**
		 *\~english
		 *\brief		Creates an element with the given informations.
		 *\param[in]	p_name		The element name.
		 *\param[in]	p_params	The parameters forwarded to the viewed manager.
		 *\return		The created element.
		 *\~french.=
		 *\brief		Crée un élément avec les informations données.
		 *\param[in]	p_name		Le nom de l'élément.
		 *\param[in]	p_params	Les paramètres transmis au gestionnaire vu.
		 *\return		L'élément créé.
		 */
		template< typename ... Params >
		inline std::shared_ptr< ResourceType > Create( Castor::String const & p_name, Params && ... p_params );
		/**
		 *\~english
		 *\brief		Removes an object, given a name.
		 *\param[in]	p_name		The element name.
		 *\param[in]	p_element	The element.
		 *\~french
		 *\brief		Retire un objet à partir d'un nom.
		 *\param[in]	p_name		Le nom d'élément.
		 *\param[in]	p_element	L'élément.
		 */
		inline void Insert( Castor::String const & p_name, std::shared_ptr< ResourceType > p_element );
		/**
		 *\~english
		 *\brief		Removes from the manager the elements created through this view.
		 *\~french
		 *\brief		Supprime du gestionnaire les éléments créés via cette vue.
		 */
		inline void Clear();
		/**
		 *\~english
		 *\param[in]	p_name	The object name.
		 *\return		\p true if an element with given name exists.
		 *\~french
		 *\param[in]	p_name	Le nom d'objet.
		 *\return		\p true Si un élément avec le nom donné existe.
		 */
		inline bool Has( Castor::String const & p_name )const;
		/**
		 *\~english
		 *\brief		Looks for an element with given name.
		 *\param[in]	p_name	The object name.
		 *\return		The found element, nullptr if not found.
		 *\~french
		 *\brief		Cherche un élément par son nom.
		 *\param[in]	p_name	Le nom d'objet.
		 *\return		L'élément trouvé, nullptr si non trouvé.
		 */
		inline std::shared_ptr< ResourceType > Find( Castor::String const & p_name )const;
		/**
		 *\~english
		 *\brief		Removes an object, given a name.
		 *\param[in]	p_name	The object name.
		 *\~french
		 *\brief		Retire un objet à partir d'un nom.
		 *\param[in]	p_name	Le nom d'objet.
		 */
		inline void Remove( Castor::String const & p_name );

	private:
		//!\~english The viewed manager.	\~french Le gestionnaire vu.
		ManagerType & m_manager;
		//!\~english The created elements names.	\~french Les noms des éléments créés.
		std::set< Castor::String > m_createdElements;
	};
}

#include "ManagerView.inl"

#endif
