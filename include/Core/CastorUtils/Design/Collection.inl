#include "CastorUtils/Log/Logger.hpp"

namespace castor
{
	namespace details
	{
		static const xchar * WARNING_COLLECTION_UNKNOWN_OBJECT = cuT( "Collection::find - Object not found: " );
		static const xchar * WARNING_COLLECTION_DUPLICATE_OBJECT = cuT( "Collection::create - Duplicate object: " );
	}

	template< typename TObj, typename TKey >
	Collection< TObj, TKey >::Collection()
		: m_locked( false )
	{
	}

	template< typename TObj, typename TKey >
	Collection< TObj, TKey >::~Collection()
	{
	}

	template< typename TObj, typename TKey >
	inline void Collection< TObj, TKey >::lock()const
	{
		m_mutex.lock();
		m_locked = true;
	}

	template< typename TObj, typename TKey >
	inline void Collection< TObj, TKey >::unlock()const
	{
		m_locked = false;
		m_mutex.unlock();
	}

	template< typename TObj, typename TKey >
	inline typename Collection< TObj, TKey >::TObjPtrMapIt Collection< TObj, TKey >::begin()
	{
		CU_Require( m_locked );
		return m_objects.begin();
	}

	template< typename TObj, typename TKey >
	inline typename Collection< TObj, TKey >::TObjPtrMapConstIt Collection< TObj, TKey >::begin()const
	{
		CU_Require( m_locked );
		return m_objects.begin();
	}

	template< typename TObj, typename TKey >
	inline typename Collection< TObj, TKey >::TObjPtrMapIt Collection< TObj, TKey >::end()
	{
		return m_objects.end();
	}

	template< typename TObj, typename TKey >
	inline typename Collection< TObj, TKey >::TObjPtrMapConstIt Collection< TObj, TKey >::end()const
	{
		return m_objects.end();
	}

	template< typename TObj, typename TKey >
	inline bool Collection< TObj, TKey >::empty()const
	{
		return m_objects.empty();
	}

	template< typename TObj, typename TKey >
	inline void Collection< TObj, TKey >::clear() throw( )
	{
		auto lock( makeUniqueLock( m_mutex ) );
		m_objects.clear();
		m_last.key = std::move( TKey() );
	}

	template< typename TObj, typename TKey >
	inline typename Collection< TObj, TKey >::TObjSPtr Collection< TObj, TKey >::find( KeyParamType key )const
	{
		auto lock( makeUniqueLock( m_mutex ) );
		TObjSPtr result;
		doUpdateLast( key );

		if ( m_last.result != m_objects.end() )
		{
			result = m_last.result->second;
		}
		else
		{
			Logger::logWarning( details::WARNING_COLLECTION_UNKNOWN_OBJECT + string::toString( key ) );
		}

		return result;
	}

	template< typename TObj, typename TKey >
	inline std::size_t Collection< TObj, TKey >::size()const
	{
		auto lock( makeUniqueLock( m_mutex ) );
		return m_objects.size();
	}

	template< typename TObj, typename TKey >
	inline bool Collection< TObj, TKey >::insert( KeyParamType key, TObjSPtr element )
	{
		auto lock( makeUniqueLock( m_mutex ) );
		TObjPtrMapIt it = m_objects.find( key );
		bool result = false;

		if ( it == m_objects.end() )
		{
			m_last.key = std::move( TKey() );
			m_objects.emplace( key, element );
			doInitLast();
			result = true;
		}
		else
		{
			Logger::logWarning( details::WARNING_COLLECTION_DUPLICATE_OBJECT + string::toString( key ) );
		}

		return result;
	}

	template< typename TObj, typename TKey >
	inline bool Collection< TObj, TKey >::has( KeyParamType key )const
	{
		auto lock( makeUniqueLock( m_mutex ) );
		doUpdateLast( key );
		return m_last.result != m_objects.end();
	}

	template< typename TObj, typename TKey >
	inline typename Collection< TObj, TKey >::TObjSPtr Collection< TObj, TKey >::erase( KeyParamType key )
	{
		auto lock( makeUniqueLock( m_mutex ) );
		TObjSPtr ret;
		TObjPtrMapIt ifind = m_objects.find( key );

		if ( ifind != m_objects.end() )
		{
			doInitLast();
			ret = ifind->second;
			m_objects.erase( key );
		}

		return ret;
	}

	template< typename TObj, typename TKey >
	typename Collection< TObj, TKey >::TObjPtrMapIt Collection< TObj, TKey >::erase( typename Collection< TObj, TKey >::TObjPtrMapIt it )
	{
		CU_Require( m_locked );
		return m_objects.erase( it );
	}

	template< typename TObj, typename TKey >
	void Collection< TObj, TKey >::doInitLast()const
	{
		m_last.key = std::move( TKey() );
		m_last.result = m_objects.end();
	}

	template< typename TObj, typename TKey >
	void Collection< TObj, TKey >::doUpdateLast( KeyParamType key )const
	{
		if ( m_last.key != key )
		{
			m_last.key = key;
			m_last.result = m_objects.find( key );
		}
	}
}
