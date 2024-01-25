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
	static const MbString ERROR_UNKNOWN_OBJECT = "Unknown object type";

	template< class KeyT
		, typename CreatorT
		, class IdT >
	struct FactoryEntryT
	{
		KeyT key;
		CreatorT create;
		IdT id;
	};

	template< class ObjT
		, class KeyT
		, class PtrTypeT
		, typename CreatorT
		, class IdT
		, class EntryT >
	class Factory
	{
	public:
		using Obj = ObjT;
		using Key = KeyT;
		using Entry = EntryT;
		using Id = IdT;
		using PtrType = PtrTypeT;
		using Creator = CreatorT;
		using ObjPtr = PtrType;
		using ObjCont = Vector< Entry >;
		struct ListEntry
		{
			Key key;
			Id id;
		};

	public:
		/**
		 *\~english
		 *\brief		Registers an object type
		 *\param[in]	key		The object type
		 *\param[in]	create	The object creation function
		 *\~french
		 *\brief		Enregistre un type d'objet
		 *\param[in]	key		Le type d'objet
		 *\param[in]	create	La fonction de création d'objet
		 */
		Entry & registerType( Key const & key, Creator create )
		{
			if ( auto it = std::find_if( m_registered.begin()
				, m_registered.end()
				, [&key]( Entry const & lookup )
				{
					return key == lookup.key;
				} );
				it != m_registered.end() )
			{
				return *it;
			}

			auto & entry = m_registered.emplace_back();
			entry.key = key;
			entry.create = create;
			entry.id = ++m_currentId;
			return entry;
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
			if ( auto it = std::find_if( m_registered.begin()
				, m_registered.end()
				, [&key]( Entry const & lookup )
				{
					return key == lookup.key;
				} );
				it != m_registered.end() )
			{
				m_registered.erase( it );
			}
		}
		/**
		 *\~english
		 *\param[in]	key	The object type.
		 *\return		The object type ID.
		 *\~french
		 *\param[in]	key	Le type d'objet.
		 *\return		L'ID du type d'objet.
		 */
		Id getTypeId( Key const & key )const
		{
			auto it = std::find_if( m_registered.begin()
				, m_registered.end()
				, [&key]( Entry const & lookup )
				{
					return key == lookup.key;
				} );

			return it == m_registered.end()
				? 0u
				: it->id;
		}
		/**
		 *\~english
		 *\param[in]	id	The object type ID.
		 *\return		The object type.
		 *\~french
		 *\param[in]	id	L'ID du type d'objet.
		 *\return		Le type d'objet.
		 */
		Key getIdType( Id const & id )const
		{
			auto it = std::find_if( m_registered.begin()
				, m_registered.end()
				, [&id]( Entry const & lookup )
				{
					return id == lookup.id;
				} );

			return it == m_registered.end()
				? Key{}
				: it->key;
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
		bool isTypeRegistered( Key const & key )const
		{
			return m_registered.end() != std::find_if( m_registered.begin()
				, m_registered.end()
				, [&key]( Entry const & lookup )
				{
					return key == lookup.key;
				} );
		}
		/**
		 *\~english
		 *\return		Retrieves the registered types list.
		 *\~french
		 *\return		La liste des types enregistrés.
		 */
		Vector< ListEntry > listRegisteredTypes()const
		{
			Vector< ListEntry > result;
			result.reserve( m_registered.size() );

			for ( auto const & entry : m_registered )
			{
				result.push_back( { entry.key, entry.id } );
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
		ObjPtr create( Key const & key
			, Parameters && ... params )const
		{
			auto it = std::find_if( m_registered.begin()
				, m_registered.end()
				, [&key]( Entry const & lookup )
				{
					return key == lookup.key;
				} );

			if ( it == m_registered.end() )
			{
				CU_Exception( ERROR_UNKNOWN_OBJECT );
			}

			return it->create( castor::forward< Parameters >( params )... );
		}

	protected:
		Id m_currentId{};
		ObjCont m_registered;
	};
}

#endif
