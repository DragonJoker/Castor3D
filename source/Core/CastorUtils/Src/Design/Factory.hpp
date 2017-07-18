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
			auto it = m_registered.find( p_key );

			if ( it != m_registered.end() )
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
			std::vector< Key > result;
			result.reserve( m_registered.size() );

			for ( auto const & it : m_registered )
			{
				result.push_back( it.first );
			}

			return result;
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
			ObjPtr result;
			auto it = m_registered.find( p_key );

			if ( it != m_registered.end() )
			{
				result = it->second( std::forward< Parameters >( p_params )... );
			}
			else
			{
				CASTOR_EXCEPTION( ERROR_UNKNOWN_OBJECT );
			}

			return result;
		}

	protected:
		ObjMap m_registered;
	};
}

#endif
