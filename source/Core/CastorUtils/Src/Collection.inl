template< typename T, typename U >
Collection< T, U >::Collection()
	:	m_bLocked( false )
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
	m_bLocked = true;
}
template< typename T, typename U >
inline void Collection< T, U >::unlock()const
{
	m_bLocked = false;
	m_mutex.unlock();
}
template< typename T, typename U >
inline typename Collection< T, U >::TObjPtrMapIt Collection< T, U >::begin()
{
	CASTOR_ASSERT( m_bLocked );
	return m_objectMap.begin();
}
template< typename T, typename U >
inline typename Collection< T, U >::TObjPtrMapConstIt Collection< T, U >::begin()const
{
	CASTOR_ASSERT( m_bLocked );
	return m_objectMap.begin();
}
template< typename T, typename U >
inline typename Collection< T, U >::TObjPtrMapIt Collection< T, U >::end()
{
	return m_objectMap.end();
}
template< typename T, typename U >
inline typename Collection< T, U >::TObjPtrMapConstIt Collection< T, U >::end()const
{
	return m_objectMap.end();
}
template< typename T, typename U >
inline void Collection< T, U >::clear() throw()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	clear_pair_container( m_objectMap );
}
template< typename T, typename U >
inline typename Collection< T, U >::TObjSPtr Collection< T, U >::find( key_param_type p_key )const
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	TObjSPtr l_pReturn;
	TObjPtrMapConstIt l_it = m_objectMap.find( p_key );

	if ( l_it != m_objectMap.end() )
	{
		l_pReturn = l_it->second;
	}

	return l_pReturn;
}
template< typename T, typename U >
inline std::size_t Collection< T, U >::size()const
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	return m_objectMap.size();
}
template< typename T, typename U >
inline bool Collection< T, U >::insert( key_param_type p_key, TObjSPtr p_element )
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	TObjPtrMapIt l_it = m_objectMap.find( p_key );
	bool l_bReturn = false;

	if ( l_it == m_objectMap.end() )
	{
		m_objectMap.insert( value_type( p_key, p_element ) );
		l_bReturn = true;
	}

	return l_bReturn;
}
template< typename T, typename U >
inline bool Collection< T, U >::has( key_param_type p_key )const
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	return m_objectMap.find( p_key ) != m_objectMap.end();
}
template< typename T, typename U >
inline typename Collection< T, U >::TObjSPtr Collection< T, U >::erase( key_param_type p_key )
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	TObjSPtr l_ret;
	TObjPtrMapIt ifind = m_objectMap.find( p_key );

	if ( ifind != m_objectMap.end() )
	{
		l_ret = ifind->second;
		m_objectMap.erase( p_key );
	}

	return l_ret;
}
