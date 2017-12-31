template< class T >
Resource< T >::Resource( String const & p_name )
	: Named( p_name )
{
}

template< class T >
Resource< T >::Resource( Resource const & p_resource )
	: Named( p_resource )
{
}

template< class T >
Resource< T >::Resource( Resource && p_resource )
	: Named( std::move( p_resource ) )
{
}

template< class T >
Resource< T > & Resource< T >::operator=( Resource const & p_resource )
{
	Named::operator=( p_resource );
	return * this;
}

template< class T >
Resource< T > & Resource< T >::operator=( Resource && p_resource )
{
	Named::operator=( std::move( p_resource ) );
	return * this;
}

template< class T >
Resource< T >::~Resource()
{
}

template< class T >
void Resource< T >::changeName( String const & p_name )
{
	collection collection;

	if ( !collection.has( p_name ) )
	{
		std::shared_ptr< T > pThis = collection.erase( m_name );

		if ( pThis )
		{
			collection.insert( p_name, pThis );
		}
	}

	m_name = p_name;
}
