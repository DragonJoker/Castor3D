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
	template<> struct FrameVariableDataTyper< TextureBase * >
	{
		C3D_API static const xchar * DataTypeName;
	};

	//*************************************************************************************************

	template< typename T >
	TFrameVariable< T >::TFrameVariable( ShaderProgramBase * p_pProgram )
		: FrameVariable( p_pProgram )
		, m_pValues( NULL )
		, m_bOwnBuffer( true )
	{
		// m_pValues initialised in child classes
	}

	template< typename T >
	TFrameVariable< T >::TFrameVariable( ShaderProgramBase * p_pProgram, uint32_t p_uiOcc )
		: FrameVariable( p_pProgram, p_uiOcc )
		, m_pValues( NULL )
		, m_bOwnBuffer( true )
	{
		// m_pValues initialised in child classes
	}

	template< typename T >
	TFrameVariable< T >::TFrameVariable( TFrameVariable< T > const & p_rVariable )
		: FrameVariable( p_rVariable )
		, m_pValues( NULL )
		, m_bOwnBuffer( p_rVariable.m_bOwnBuffer )
	{
		if ( p_rVariable.m_bOwnBuffer )
		{
			uint8_t * l_pBuffer = new uint8_t[p_rVariable.size()];
			m_pValues = reinterpret_cast< T * >( l_pBuffer );
			memcpy( m_pValues, p_rVariable.m_pValues, p_rVariable.size() );
		}
		else
		{
			m_pValues = p_rVariable.m_pValues;
		}

		for ( uint32_t i = 0; i < m_uiOcc; i++ )
		{
			m_strValue[i] = p_rVariable.m_strValue[i];
		}
	}

	template< typename T >
	TFrameVariable< T >::TFrameVariable( TFrameVariable< T > && p_rVariable )
		: FrameVariable( std::move( p_rVariable ) )
		, m_pValues( std::move( p_rVariable.m_pValues ) )
		, m_bOwnBuffer( std::move( p_rVariable.m_bOwnBuffer ) )
	{
		for ( uint32_t i = 0; i < m_uiOcc; i++ )
		{
			m_strValue[i] = std::move( p_rVariable.m_strValue[i] );
			p_rVariable.m_strValue[i].clear();
		}

		p_rVariable.m_pValues = NULL;
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
			m_pValues = std::move( p_rVariable.m_pValues );

			for ( uint32_t i = 0; i < m_uiOcc; i++ )
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
		return reinterpret_cast< uint8_t * >( m_pValues );
	}

	template< typename T >
	void TFrameVariable< T >::link( uint8_t * p_pBuffer )
	{
		memcpy( p_pBuffer, m_pValues, size() );
		DoCleanupBuffer();
		m_pValues = reinterpret_cast< T * >( p_pBuffer );
		m_bOwnBuffer = false;
	}

	template< typename T >
	inline void TFrameVariable< T >::DoCleanupBuffer()
	{
		if ( m_bOwnBuffer )
		{
			delete [] m_pValues;
		}
	}
}
