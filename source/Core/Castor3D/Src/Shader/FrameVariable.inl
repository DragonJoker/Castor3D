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
	template<> struct FrameVariableDataTyper< TextureLayout * >
	{
		C3D_API static const xchar * DataTypeName;
	};

	//*************************************************************************************************

	template< typename T >
	TFrameVariable< T >::TFrameVariable( ShaderProgram & p_program )
		: FrameVariable( p_program )
		, m_values( nullptr )
		, m_bOwnBuffer( true )
	{
		// m_values initialised in child classes
	}

	template< typename T >
	TFrameVariable< T >::TFrameVariable( ShaderProgram & p_program, uint32_t p_occurences )
		: FrameVariable( p_program, p_occurences )
		, m_values( nullptr )
		, m_bOwnBuffer( true )
	{
		// m_values initialised in child classes
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
