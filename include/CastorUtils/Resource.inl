template <class T>
Resource<T> :: Resource( String const & p_name)
	:	m_strName( p_name)
{
}
template <class T>
Resource<T> :: Resource( Resource<T> const & p_resource)
	:	m_strName( p_resource.m_strName)
{
}
template <class T>
Resource<T> & Resource<T> :: operator =( Resource<T> const & p_resource)
{
	m_strName = p_resource.m_strName;
	return * this;
}
template <class T>
Resource<T> :: ~Resource()
{
}
template <class T>
void Resource<T> :: SetName( String const & p_strName)
{
	collection l_collection;

	if ( ! l_collection.HasElement( p_strName))
	{
		shared_ptr<T> l_pThis = l_collection.RemoveElement( m_strName);

		if (l_pThis)
		{
			m_strName = p_strName;
			l_collection.AddElement( p_strName, l_pThis);
		}
	}
}