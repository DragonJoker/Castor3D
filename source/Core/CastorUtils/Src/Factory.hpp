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
#ifndef ___CASTOR_FACTORY_H___
#define ___CASTOR_FACTORY_H___

#include "NonCopyable.hpp"

#include <type_traits>
#include <functional>

namespace Castor
{
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
	template< class Obj, class Key, typename Creator, class Predicate >
	class Factory
		: public Castor::NonCopyable
	{
	protected:
		typedef std::shared_ptr< Obj > ObjPtr;
		typedef std::map< Key, Creator, Predicate > ObjMap;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Factory() {}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Factory() {}
		/**
		 *\~english
		 *\brief		Registers all objects types
		 *\~french
		 *\brief		Enregistre tous les types d'objets
		 */
		virtual void Initialise() = 0;
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
			m_registered.insert( std::make_pair( p_key, p_creator ) );
		}
#if CASTOR_HAS_VARIADIC_TEMPLATES
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
		ObjPtr Create( Key const & p_key, Parameters && ... p_params )
		{
			ObjPtr l_return;
			typename ObjMap::iterator l_it = m_registered.find( p_key );

			if ( l_it != m_registered.end() )
			{
				l_return = l_it->second( std::forward< Parameters >( p_params )... );
			}

			return l_return;
		}
#endif

	protected:
		ObjMap m_registered;
	};
}

#endif
