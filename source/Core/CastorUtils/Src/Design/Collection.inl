#include "Log/Logger.hpp"

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
		REQUIRE( m_locked );
		return m_objects.begin();
	}
	template< typename TObj, typename TKey >
	inline typename Collection< TObj, TKey >::TObjPtrMapConstIt Collection< TObj, TKey >::begin()const
	{
		REQUIRE( m_locked );
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
		m_last.m_key = std::move( TKey() );
	}
	template< typename TObj, typename TKey >
	inline typename Collection< TObj, TKey >::TObjSPtr Collection< TObj, TKey >::find( KeyParamType p_key )const
	{
		auto lock( makeUniqueLock( m_mutex ) );
		TObjSPtr result;
		doUpdateLast( p_key );

		if ( m_last.m_result != m_objects.end() )
		{
			result = m_last.m_result->second;
		}
		else
		{
			Logger::logWarning( details::WARNING_COLLECTION_UNKNOWN_OBJECT + string::toString( p_key ) );
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
	inline bool Collection< TObj, TKey >::insert( KeyParamType p_key, TObjSPtr p_element )
	{
		auto lock( makeUniqueLock( m_mutex ) );
		TObjPtrMapIt it = m_objects.find( p_key );
		bool result = false;

		if ( it == m_objects.end() )
		{
			m_last.m_key = std::move( TKey() );
			m_objects.emplace( p_key, p_element );
			doInitLast();
			result = true;
		}
		else
		{
			Logger::logWarning( details::WARNING_COLLECTION_DUPLICATE_OBJECT + string::toString( p_key ) );
		}

		return result;
	}
	template< typename TObj, typename TKey >
	inline bool Collection< TObj, TKey >::has( KeyParamType p_key )const
	{
		auto lock( makeUniqueLock( m_mutex ) );
		doUpdateLast( p_key );
		return m_last.m_result != m_objects.end();
	}
	template< typename TObj, typename TKey >
	inline typename Collection< TObj, TKey >::TObjSPtr Collection< TObj, TKey >::erase( KeyParamType p_key )
	{
		auto lock( makeUniqueLock( m_mutex ) );
		TObjSPtr ret;
		TObjPtrMapIt ifind = m_objects.find( p_key );

		if ( ifind != m_objects.end() )
		{
			doInitLast();
			ret = ifind->second;
			m_objects.erase( p_key );
		}

		return ret;
	}

	template< typename TObj, typename TKey >
	void Collection< TObj, TKey >::doInitLast()const
	{
		m_last.m_key = std::move( TKey() );
		m_last.m_result = m_objects.end();
	}

	template< typename TObj, typename TKey >
	void Collection< TObj, TKey >::doUpdateLast( KeyParamType p_key )const
	{
		if ( m_last.m_key != p_key )
		{
			m_last.m_key = p_key;
			m_last.m_result = m_objects.find( p_key );
		}
	}
}
