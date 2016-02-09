namespace Castor3D
{
	template<> struct FrameVariableDataTyper< bool >
	{
		C3D_API static const xchar * DataTypeName;
	};
	template<> struct FrameVariableDataTyper< int >
	{
		C3D_API static const xchar * DataTypeName;
	};
	template<> struct FrameVariableDataTyper< uint32_t >
	{
		C3D_API static const xchar * DataTypeName;
	};
	template<> struct FrameVariableDataTyper< float >
	{
		C3D_API static const xchar * DataTypeName;
	};
	template<> struct FrameVariableDataTyper< double >
	{
		C3D_API static const xchar * DataTypeName;
	};
	template<> struct FrameVariableDataTyper< Texture * >
	{
		C3D_API static const xchar * DataTypeName;
	};

	//*************************************************************************************************

	template< typename T >
	TFrameVariable< T >::TFrameVariable( ShaderProgram * p_program )
		: FrameVariable( p_program )
		, m_values( NULL )
		, m_bOwnBuffer( true )
	{
		// m_values initialised in child classes
	}

	template< typename T >
	TFrameVariable< T >::TFrameVariable( ShaderProgram * p_program, uint32_t p_occurences )
		: FrameVariable( p_program, p_occurences )
		, m_values( NULL )
		, m_bOwnBuffer( true )
	{
		// m_values initialised in child classes
	}

	template< typename T >
	TFrameVariable< T >::TFrameVariable( TFrameVariable< T > const & p_rVariable )
		: FrameVariable( p_rVariable )
		, m_values( NULL )
		, m_bOwnBuffer( p_rVariable.m_bOwnBuffer )
	{
		if ( p_rVariable.m_bOwnBuffer )
		{
			uint8_t * l_pBuffer = new uint8_t[p_rVariable.size()];
			m_values = reinterpret_cast< T * >( l_pBuffer );
			memcpy( m_values, p_rVariable.m_values, p_rVariable.size() );
		}
		else
		{
			m_values = p_rVariable.m_values;
		}

		for ( uint32_t i = 0; i < m_occurences; i++ )
		{
			m_strValue[i] = p_rVariable.m_strValue[i];
		}
	}

	template< typename T >
	TFrameVariable< T >::TFrameVariable( TFrameVariable< T > && p_rVariable )
		: FrameVariable( std::move( p_rVariable ) )
		, m_values( std::move( p_rVariable.m_values ) )
		, m_bOwnBuffer( std::move( p_rVariable.m_bOwnBuffer ) )
	{
		for ( uint32_t i = 0; i < m_occurences; i++ )
		{
			m_strValue[i] = std::move( p_rVariable.m_strValue[i] );
			p_rVariable.m_strValue[i].clear();
		}

		p_rVariable.m_values = NULL;
	}

	template< typename T >
	TFrameVariable< T > & TFrameVariable< T >::operator =( TFrameVariable< T > const & p_rVariable )
	{
		TFrameVariable< T > l_tmp( p_rVariable );
		std::swap( *this, l_tmp );
		return *this;
	}

	template< typename T >
	TFrameVariable< T > & TFrameVariable< T >::operator =( TFrameVariable< T > && p_rVariable )
	{
		FrameVariable::operator =( std::move( p_rVariable ) );

		if ( this != &p_rVariable )
		{
			DoCleanupBuffer();
			m_bOwnBuffer = std::move( p_rVariable.m_bOwnBuffer );
			m_values = std::move( p_rVariable.m_values );

			for ( uint32_t i = 0; i < m_occurences; i++ )
			{
				m_strValue[i] = std::move( p_rVariable.m_strValue[i] );
				p_rVariable.m_strValue[i].clear();
			}
		}

		return *this;
	}

	template< typename T >
	TFrameVariable< T >::~TFrameVariable()
	{
		DoCleanupBuffer();
	}

	template< typename T >
	Castor::String TFrameVariable< T >::GetDataTypeName()const
	{
		return FrameVariableDataTyper< T >::DataTypeName;
	}

	template< typename T >
	uint8_t const * const TFrameVariable< T >::const_ptr()const
	{
		return reinterpret_cast< uint8_t * >( m_values );
	}

	template< typename T >
	void TFrameVariable< T >::link( uint8_t * p_buffer )
	{
		memcpy( p_buffer, m_values, size() );
		DoCleanupBuffer();
		m_values = reinterpret_cast< T * >( p_buffer );
		m_bOwnBuffer = false;
	}

	template< typename T >
	inline void TFrameVariable< T >::DoCleanupBuffer()
	{
		if ( m_bOwnBuffer )
		{
			delete [] m_values;
		}
	}
}
