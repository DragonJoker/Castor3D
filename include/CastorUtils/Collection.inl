template< typename T, typename U >
Collection<T, U> :: Collection( Collection<T, U> const & p_manager)
{
}
template< typename T, typename U >
Collection<T, U> & Collection<T, U> :: operator =( Collection<T, U> const &)
{
	return * this;
}
template< typename T, typename U >
Collection<T, U> :: Collection()
	:	m_bLocked( false)
{
}
template< typename T, typename U >
Collection<T, U> :: ~Collection()
{
}
template< typename T, typename U >
inline void Collection<T, U> :: Lock()const
{
	m_mutex.Lock();
	m_bLocked = true;
}
template< typename T, typename U >
inline void Collection<T, U> :: Unlock()const
{
	m_bLocked = false;
	m_mutex.Unlock();
}
template< typename T, typename U >
inline typename Collection<T, U>::iterator Collection<T, U> :: Begin()
{
	CASTOR_ASSERT( m_bLocked);
	return m_objectMap.begin();
}
template< typename T, typename U >
inline typename Collection<T, U>::const_iterator Collection<T, U> :: Begin()const
{
	CASTOR_ASSERT( m_bLocked);
	return m_objectMap.begin();
}
template< typename T, typename U >
inline typename Collection<T, U>::const_iterator Collection<T, U> :: End()const
{
	return m_objectMap.end();
}
template< typename T, typename U >
inline void Collection<T, U> :: Clear() throw()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	ClearContainer( m_objectMap);
}
template< typename T, typename U >
inline typename Collection<T, U>::obj_ptr Collection<T, U> :: GetElement( key_param_type p_key)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	obj_ptr l_pReturn;
	iterator l_it = m_objectMap.find( p_key);

	if (l_it != m_objectMap.end())
	{
		l_pReturn = l_it->second;
	}

	return l_pReturn;
}
template< typename T, typename U >
inline size_t Collection<T, U> :: GetElementCount()const
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	return m_objectMap.size();
}
template< typename T, typename U >
inline bool Collection<T, U> :: AddElement( key_param_type p_key, obj_ptr p_element)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	iterator l_it = m_objectMap.find( p_key);
	bool l_bReturn = false;

	if (l_it == m_objectMap.end())
	{
		m_objectMap.insert( value_type( p_key, p_element));
		l_bReturn = true;
	}

	return l_bReturn;
}
template< typename T, typename U >
inline bool Collection<T, U> :: HasElement( key_param_type p_key)const
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	return m_objectMap.find( p_key) != m_objectMap.end();
}
template< typename T, typename U >
inline typename Collection<T, U>::obj_ptr Collection<T, U> :: RemoveElement( key_param_type p_key)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	obj_ptr l_ret;
	const_iterator ifind = m_objectMap.find( p_key);

	if (ifind != m_objectMap.end())
	{
		l_ret = ifind->second;
		m_objectMap.erase( p_key);
	}

	return l_ret;
}