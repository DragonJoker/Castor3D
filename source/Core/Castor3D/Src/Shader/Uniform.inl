namespace Castor3D
{
	template<> struct UniformDataTyper< bool >
	{
		C3D_API static const Castor::String DataTypeName;
	};
	template<> struct UniformDataTyper< int >
	{
		C3D_API static const Castor::String DataTypeName;
	};
	template<> struct UniformDataTyper< uint32_t >
	{
		C3D_API static const Castor::String DataTypeName;
	};
	template<> struct UniformDataTyper< float >
	{
		C3D_API static const Castor::String DataTypeName;
	};
	template<> struct UniformDataTyper< double >
	{
		C3D_API static const Castor::String DataTypeName;
	};
	template<> struct UniformDataTyper< TextureLayout * >
	{
		C3D_API static const Castor::String DataTypeName;
	};

	//*************************************************************************************************

	template< typename T >
	TUniform< T >::TUniform( ShaderProgram & p_program, uint32_t p_occurences )
		: Uniform( p_program, p_occurences )
		, m_values( nullptr )
		, m_bOwnBuffer( true )
	{
		// m_values initialised in child classes.
	}

	template< typename T >
	TUniform< T >::~TUniform()
	{
		DoCleanupBuffer();
	}

	template< typename T >
	Castor::String const & TUniform< T >::GetDataTypeName()const
	{
		return UniformDataTyper< T >::DataTypeName;
	}

	template< typename T >
	uint8_t const * const TUniform< T >::const_ptr()const
	{
		return reinterpret_cast< uint8_t * >( m_values );
	}

	template< typename T >
	inline void TUniform< T >::DoCleanupBuffer()
	{
		if ( m_bOwnBuffer )
		{
			delete [] m_values;
			m_stride = 0u;
		}
	}
}
