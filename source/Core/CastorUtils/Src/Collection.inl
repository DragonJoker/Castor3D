#include "Logger.hpp"

namespace Castor
{
	namespace
	{
		static const xchar * WARNING_COLLECTION_UNKNOWN_OBJECT = cuT( "Collection::find - Object not found: " );
		static const xchar * WARNING_COLLECTION_DUPLICATE_OBJECT = cuT( "Collection::Create - Duplicate object: " );
	}

	template< typename T, typename U >
	Collection< T, U >::Collection()
		: m_locked( false )
	{
	}
	template< typename T, typename U >
	Collection< T, U >::~Collection()
	{
	}
	template< typename T, typename U >
	inline void Collection< T, U >::lock()const
	{
		m_mutex.lock();
		m_locked = true;
	}
	template< typename T, typename U >
	inline void Collection< T, U >::unlock()const
	{
		m_locked = false;
		m_mutex.unlock();
	}
	template< typename T, typename U >
	inline typename Collection< T, U >::TObjPtrMapIt Collection< T, U >::begin()
	{
		CASTOR_ASSERT( m_locked );
		return m_objects.begin();
	}
	template< typename T, typename U >
	inline typename Collection< T, U >::TObjPtrMapConstIt Collection< T, U >::begin()const
	{
		CASTOR_ASSERT( m_locked );
		return m_objects.begin();
	}
	template< typename T, typename U >
	inline typename Collection< T, U >::TObjPtrMapIt Collection< T, U >::end()
	{
		return m_objects.end();
	}
	template< typename T, typename U >
	inline typename Collection< T, U >::TObjPtrMapConstIt Collection< T, U >::end()const
	{
		return m_objects.end();
	}
	template< typename T, typename U >
	inline void Collection< T, U >::clear() throw( )
	{
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		clear_pair_container( m_objects );
	}
	template< typename T, typename U >
	inline typename Collection< T, U >::TObjSPtr Collection< T, U >::find( key_param_type p_key )const
	{
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		TObjSPtr l_pReturn;
		TObjPtrMapConstIt l_it = m_objects.find( p_key );

		if ( l_it != m_objects.end() )
		{
			l_pReturn = l_it->second;
		}
		else
		{
			Logger::LogWarning( WARNING_COLLECTION_UNKNOWN_OBJECT + string::to_string( p_key ) );
		}

		return l_pReturn;
	}
	template< typename T, typename U >
	inline std::size_t Collection< T, U >::size()const
	{
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		return m_objects.size();
	}
	template< typename T, typename U >
	inline bool Collection< T, U >::insert( key_param_type p_key, TObjSPtr p_element )
	{
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		TObjPtrMapIt l_it = m_objects.find( p_key );
		bool l_return = false;

		if ( l_it == m_objects.end() )
		{
			m_objects.insert( value_type( p_key, p_element ) );
			l_return = true;
		}
		else
		{
			Logger::LogWarning( WARNING_COLLECTION_DUPLICATE_OBJECT + string::to_string( p_key ) );
		}

		return l_return;
	}
	template< typename T, typename U >
	inline bool Collection< T, U >::has( key_param_type p_key )const
	{
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		return m_objects.find( p_key ) != m_objects.end();
	}
	template< typename T, typename U >
	inline typename Collection< T, U >::TObjSPtr Collection< T, U >::erase( key_param_type p_key )
	{
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		TObjSPtr l_ret;
		TObjPtrMapIt ifind = m_objects.find( p_key );

		if ( ifind != m_objects.end() )
		{
			l_ret = ifind->second;
			m_objects.erase( p_key );
		}

		return l_ret;
	}}
