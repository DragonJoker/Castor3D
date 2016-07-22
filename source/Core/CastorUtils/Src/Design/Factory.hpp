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
#ifndef ___CASTOR_FACTORY_H___
#define ___CASTOR_FACTORY_H___

#include "NonCopyable.hpp"
#include "Exception/Exception.hpp"

#include <type_traits>
#include <functional>

namespace Castor
{
	static const std::string ERROR_UNKNOWN_OBJECT = "Unknown object type";
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Factory concept implementation
	\remark		The classes that can be registered must implement a function of the following form :
				<br />static std::shared_ptr< Obj > Create();
	\~french
	\brief		Implémentation du concept de fabrique
	\remark		Les classes pouvant être enregistrées doivent implémenter une fonction de la forme suivante :
				<br />static std::shared_ptr< Obj > Create();
	*/
	template< class Obj, class Key, class PtrType, typename Creator, class Predicate >
	class Factory
	{
	protected:
		typedef PtrType ObjPtr;
		typedef std::map< Key, Creator, Predicate > ObjMap;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Factory() = default;
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Factory() = default;
		/**
		 *\~english
		 *\brief		Registers an object type
		 *\param[in]	p_key		The object type
		 *\param[in]	p_creator	The object creation function
		 *\~french
		 *\brief		Enregistre un type d'objet
		 *\param[in]	p_key		Le type d'objet
		 *\param[in]	p_creator	La fonction de création d'objet
		 */
		void Register( Key const & p_key, Creator p_creator )
		{
			m_registered[p_key] = p_creator;
		}
		/**
		 *\~english
		 *\brief		Unregisters an object type
		 *\param[in]	p_key		The object type
		 *\~french
		 *\brief		Désenregistre un type d'objet
		 *\param[in]	p_key		Le type d'objet
		 */
		void Unregister( Key const & p_key )
		{
			auto l_it = m_registered.find( p_key );

			if ( l_it != m_registered.end() )
			{
				m_registered.erase( p_key );
			}
		}
		/**
		 *\~english
		 *\brief		Checks if the given object type is registered.
		 *\param[in]	p_key	The object type.
		 *\return		\p true if registered.
		 *\~french
		 *\brief		Vérifie si un type d'objet est enregistré.
		 *\param[in]	p_key	Le type d'objet.
		 *\return		\p true si enregistré.
		 */
		bool IsRegistered( Key const & p_key )
		{
			return m_registered.find( p_key ) != m_registered.end();
		}
		/**
		 *\~english
		 *\return		Retrieves the registered types list.
		 *\~french
		 *\return		La liste des types enregistrés.
		 */
		std::vector< Key > ListRegisteredTypes()
		{
			std::vector< Key > l_return;
			l_return.reserve( m_registered.size() );

			for ( auto const & l_it : m_registered )
			{
				l_return.push_back( l_it.first );
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		Creates an object from a key
		 *\param[in]	p_key		The object type
		 *\param[in]	p_params	The creation parameters
		 *\return		The created object
		 *\~french
		 *\brief		Crée un objet à partir d'une clef (type d'objet)
		 *\param[in]	p_key		Le type d'objet
		 *\param[in]	p_params	Les paramètres de création
		 *\return		L'objet créé
		 */
		template< typename ... Parameters >
		ObjPtr Create( Key const & p_key, Parameters && ... p_params )const
		{
			ObjPtr l_return;
			auto l_it = m_registered.find( p_key );

			if ( l_it != m_registered.end() )
			{
				l_return = l_it->second( std::forward< Parameters >( p_params )... );
			}
			else
			{
				CASTOR_EXCEPTION( ERROR_UNKNOWN_OBJECT );
			}

			return l_return;
		}

	protected:
		ObjMap m_registered;
	};
}

#endif
