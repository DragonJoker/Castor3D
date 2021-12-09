/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_FACTORY_H___
#define ___CASTOR_FACTORY_H___

#include "CastorUtils/Design/DesignModule.hpp"

#include "CastorUtils/Design/NonCopyable.hpp"
#include "CastorUtils/Exception/Exception.hpp"

#include <type_traits>
#include <functional>

namespace castor
{
	static const std::string ERROR_UNKNOWN_OBJECT = "Unknown object type";

	template< class ObjT, class KeyT, class PtrTypeT, typename CreatorT, class PredicateT >
	class Factory
	{
	public:
		using Obj = ObjT;
		using Key = KeyT;
		using PtrType = PtrTypeT;
		using Creator = CreatorT;
		using Predicate = PredicateT;
		using ObjPtr = PtrType ;
		using ObjMap = std::map< Key, Creator, Predicate >;

	public:
		/**
		 *\~english
		 *\brief		Registers an object type
		 *\param[in]	key		The object type
		 *\param[in]	creator	The object creation function
		 *\~french
		 *\brief		Enregistre un type d'objet
		 *\param[in]	key		Le type d'objet
		 *\param[in]	creator	La fonction de création d'objet
		 */
		void registerType( Key const & key, Creator creator )
		{
			m_registered[key] = creator;
		}
		/**
		 *\~english
		 *\brief		Unregisters an object type
		 *\param[in]	key	The object type
		 *\~french
		 *\brief		Désenregistre un type d'objet
		 *\param[in]	key	Le type d'objet
		 */
		void unregisterType( Key const & key )
		{
			auto it = m_registered.find( key );

			if ( it != m_registered.end() )
			{
				m_registered.erase( key );
			}
		}
		/**
		 *\~english
		 *\brief		Checks if the given object type is registered.
		 *\param[in]	key	The object type.
		 *\return		\p true if registered.
		 *\~french
		 *\brief		Vérifie si un type d'objet est enregistré.
		 *\param[in]	key	Le type d'objet.
		 *\return		\p true si enregistré.
		 */
		bool isTypeRegistered( Key const & key )
		{
			return m_registered.find( key ) != m_registered.end();
		}
		/**
		 *\~english
		 *\return		Retrieves the registered types list.
		 *\~french
		 *\return		La liste des types enregistrés.
		 */
		std::vector< Key > listRegisteredTypes()
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
		 *\param[in]	key		The object type
		 *\param[in]	params	The creation parameters
		 *\return		The created object
		 *\~french
		 *\brief		Crée un objet à partir d'une clef (type d'objet)
		 *\param[in]	key		Le type d'objet
		 *\param[in]	params	Les paramètres de création
		 *\return		L'objet créé
		 */
		template< typename ... Parameters >
		ObjPtr create( Key const & key, Parameters && ... params )const
		{
			ObjPtr result;
			auto it = m_registered.find( key );

			if ( it != m_registered.end() )
			{
				result = it->second( std::forward< Parameters >( params )... );
			}
			else
			{
				CU_Exception( ERROR_UNKNOWN_OBJECT );
			}

			return result;
		}

	protected:
		ObjMap m_registered;
	};
}

#endif
