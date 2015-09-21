template< class T >
Resource< T >::Resource( String const & p_name )
	:	NamedBase< String >	( p_name	)
{
}

template< class T >
Resource< T >::Resource( Resource const & p_resource )
	:	NamedBase< String >	( p_resource	)
{
}

template< class T >
Resource< T >::Resource( Resource && p_resource )
	:	NamedBase< String >	( std::move( p_resource )	)
{
}

template< class T >
Resource< T > & Resource< T >::operator =( Resource const & p_resource )
{
	NamedBase< String >::operator =( p_resource );
	return * this;
}

template< class T >
Resource< T > & Resource< T >::operator =( Resource && p_resource )
{
	NamedBase< String >::operator =( std::move( p_resource ) );
	return * this;
}

template< class T >
Resource< T >::~Resource()
{
}

template< class T >
void Resource< T >::ChangeName( String const & p_name )
{
	collection l_collection;

	if ( !l_collection.has( p_name ) )
	{
		std::shared_ptr< T > l_pThis = l_collection.erase( m_name );

		if ( l_pThis )
		{
			l_collection.insert( p_name, l_pThis );
		}
	}

	m_name = p_name;
}
