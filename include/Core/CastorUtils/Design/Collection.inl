#include "CastorUtils/Log/Logger.hpp"

namespace castor
{
	namespace details
	{
		static const xchar * WARNING_COLLECTION_UNKNOWN_OBJECT = cuT( "Collection::find - Object not found: " );
		static const xchar * WARNING_COLLECTION_DUPLICATE_OBJECT = cuT( "Collection::create - Duplicate object: " );
	}

	template< typename ObjectT, typename KeyT >
	Collection< ObjectT, KeyT >::Collection( LoggerInstance & logger )
		: m_logger( logger )
		, m_locked( false )
	{
	}

	template< typename ObjectT, typename KeyT >
	Collection< ObjectT, KeyT >::~Collection()
	{
	}

	template< typename ObjectT, typename KeyT >
	inline LoggerInstance & Collection< ObjectT, KeyT >::getLogger()const
	{
		return m_logger;
	}

	template< typename ObjectT, typename KeyT >
	inline void Collection< ObjectT, KeyT >::lock()const
	{
		m_mutex.lock();
		m_locked = true;
	}

	template< typename ObjectT, typename KeyT >
	inline void Collection< ObjectT, KeyT >::unlock()const
	{
		m_locked = false;
		m_mutex.unlock();
	}

	template< typename ObjectT, typename KeyT >
	inline typename Collection< ObjectT, KeyT >::ObjectPtrTMapIt Collection< ObjectT, KeyT >::begin()
	{
		CU_Require( m_locked );
		return m_objects.begin();
	}

	template< typename ObjectT, typename KeyT >
	inline typename Collection< ObjectT, KeyT >::ObjectPtrTMapConstIt Collection< ObjectT, KeyT >::begin()const
	{
		CU_Require( m_locked );
		return m_objects.begin();
	}

	template< typename ObjectT, typename KeyT >
	inline typename Collection< ObjectT, KeyT >::ObjectPtrTMapIt Collection< ObjectT, KeyT >::end()
	{
		return m_objects.end();
	}

	template< typename ObjectT, typename KeyT >
	inline typename Collection< ObjectT, KeyT >::ObjectPtrTMapConstIt Collection< ObjectT, KeyT >::end()const
	{
		return m_objects.end();
	}

	template< typename ObjectT, typename KeyT >
	inline bool Collection< ObjectT, KeyT >::empty()const
	{
		return m_objects.empty();
	}

	template< typename ObjectT, typename KeyT >
	inline void Collection< ObjectT, KeyT >::clear() throw( )
	{
		auto lock( makeUniqueLock( m_mutex ) );
		m_objects.clear();
		doInitLast();
	}

	template< typename ObjectT, typename KeyT >
	inline typename Collection< ObjectT, KeyT >::ObjectTSPtr Collection< ObjectT, KeyT >::find( KeyParamType key )const
	{
		auto lock( makeUniqueLock( m_mutex ) );
		ObjectTSPtr result;
		doUpdateLast( key );

		if ( m_last.result != m_objects.end() )
		{
			result = m_last.result->second;
		}
		else
		{
			m_logger.logWarning( details::WARNING_COLLECTION_UNKNOWN_OBJECT + string::toString( key ) );
		}

		return result;
	}

	template< typename ObjectT, typename KeyT >
	inline std::size_t Collection< ObjectT, KeyT >::size()const
	{
		auto lock( makeUniqueLock( m_mutex ) );
		return m_objects.size();
	}

	template< typename ObjectT, typename KeyT >
	inline bool Collection< ObjectT, KeyT >::insert( KeyParamType key, ObjectTSPtr element )
	{
		auto lock( makeUniqueLock( m_mutex ) );
		doUpdateLast( key );
		bool result = false;

		if ( m_last.result == m_objects.end() )
		{
			m_last.result = m_objects.emplace( key, element ).first;
			result = true;
		}
		else
		{
			m_logger.logWarning( details::WARNING_COLLECTION_DUPLICATE_OBJECT + string::toString( key ) );
		}

		return result;
	}

	template< typename ObjectT, typename KeyT >
	inline bool Collection< ObjectT, KeyT >::has( KeyParamType key )const
	{
		auto lock( makeUniqueLock( m_mutex ) );
		doUpdateLast( key );
		return m_last.result != m_objects.end();
	}

	template< typename ObjectT, typename KeyT >
	inline typename Collection< ObjectT, KeyT >::ObjectTSPtr Collection< ObjectT, KeyT >::erase( KeyParamType key )
	{
		auto lock( makeUniqueLock( m_mutex ) );
		doUpdateLast( key );
		ObjectTSPtr ret;

		if ( m_last.result != m_objects.end() )
		{
			ret = m_last.result->second;
			m_objects.erase( m_last.result );
			doInitLast();
		}

		return ret;
	}

	template< typename ObjectT, typename KeyT >
	typename Collection< ObjectT, KeyT >::ObjectPtrTMapIt Collection< ObjectT, KeyT >::erase( typename Collection< ObjectT, KeyT >::ObjectPtrTMapIt it )
	{
		CU_Require( m_locked );
		return m_objects.erase( it );
	}

	template< typename ObjectT, typename KeyT >
	void Collection< ObjectT, KeyT >::doInitLast()const
	{
		m_last.key = std::move( KeyT() );
		m_last.result = m_objects.end();
	}

	template< typename ObjectT, typename KeyT >
	void Collection< ObjectT, KeyT >::doUpdateLast( KeyParamType key )const
	{
		if ( m_last.key != key )
		{
			m_last.key = key;
			m_last.result = m_objects.find( key );
		}
	}
}
