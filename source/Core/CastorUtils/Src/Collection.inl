#include "Logger.hpp"

namespace Castor
{
	namespace
	{
		static const xchar * WARNING_COLLECTION_UNKNOWN_OBJECT = cuT( "Collection::find - Object not found: " );
		static const xchar * WARNING_COLLECTION_DUPLICATE_OBJECT = cuT( "Collection::Create - Duplicate object: " );
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
	inline void Collection< TObj, TKey >::clear() throw( )
	{
		auto l_lock( make_unique_lock( m_mutex ) );
		m_objects.clear();
		m_last.m_key = std::move( TKey() );
	}
	template< typename TObj, typename TKey >
	inline typename Collection< TObj, TKey >::TObjSPtr Collection< TObj, TKey >::find( key_param_type p_key )const
	{
		auto l_lock( make_unique_lock( m_mutex ) );
		TObjSPtr l_return;
		do_update_last( p_key );

		if ( m_last.m_result != m_objects.end() )
		{
			l_return = m_last.m_result->second;
		}
		else
		{
			Logger::LogWarning( WARNING_COLLECTION_UNKNOWN_OBJECT + string::to_string( p_key ) );
		}

		return l_return;
	}
	template< typename TObj, typename TKey >
	inline std::size_t Collection< TObj, TKey >::size()const
	{
		auto l_lock( make_unique_lock( m_mutex ) );
		return m_objects.size();
	}
	template< typename TObj, typename TKey >
	inline bool Collection< TObj, TKey >::insert( key_param_type p_key, TObjSPtr p_element )
	{
		auto l_lock( make_unique_lock( m_mutex ) );
		TObjPtrMapIt l_it = m_objects.find( p_key );
		bool l_return = false;

		if ( l_it == m_objects.end() )
		{
			m_last.m_key = std::move( TKey() );
			m_objects.insert( value_type( p_key, p_element ) );
			do_init_last();
			l_return = true;
		}
		else
		{
			Logger::LogWarning( WARNING_COLLECTION_DUPLICATE_OBJECT + string::to_string( p_key ) );
		}

		return l_return;
	}
	template< typename TObj, typename TKey >
	inline bool Collection< TObj, TKey >::has( key_param_type p_key )const
	{
		auto l_lock( make_unique_lock( m_mutex ) );
		do_update_last( p_key );
		return m_last.m_result != m_objects.end();
	}
	template< typename TObj, typename TKey >
	inline typename Collection< TObj, TKey >::TObjSPtr Collection< TObj, TKey >::erase( key_param_type p_key )
	{
		auto l_lock( make_unique_lock( m_mutex ) );
		TObjSPtr l_ret;
		TObjPtrMapIt ifind = m_objects.find( p_key );

		if ( ifind != m_objects.end() )
		{
			do_init_last();
			l_ret = ifind->second;
			m_objects.erase( p_key );
		}

		return l_ret;
	}

	template< typename TObj, typename TKey >
	void Collection< TObj, TKey >::do_init_last()const
	{
		m_last.m_key = std::move( TKey() );
		m_last.m_result = m_objects.end();
	}

	template< typename TObj, typename TKey >
	void Collection< TObj, TKey >::do_update_last( key_param_type p_key )const
	{
		if ( m_last.m_key != p_key )
		{
			m_last.m_key = p_key;
			m_last.m_result = m_objects.find( p_key );
		}
	}
}
