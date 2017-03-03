namespace GlES3Render
{
	//**********************************************************************************************

	template< typename T > struct GlES3Typer;

	template <> struct GlES3Typer< int >
	{
		constexpr static GlES3Type Value = GlES3Type::eInt;
	};
	template <> struct GlES3Typer< uint32_t >
	{
		constexpr static GlES3Type Value = GlES3Type::eUnsignedInt;
	};
	template <> struct GlES3Typer< short >
	{
		constexpr static GlES3Type Value = GlES3Type::eShort;
	};
	template <> struct GlES3Typer< uint16_t >
	{
		constexpr static GlES3Type Value = GlES3Type::eUnsignedShort;
	};
	template <> struct GlES3Typer< char >
	{
		constexpr static GlES3Type Value = GlES3Type::eByte;
	};
	template <> struct GlES3Typer< uint8_t >
	{
		constexpr static GlES3Type Value = GlES3Type::eUnsignedByte;
	};
	template <> struct GlES3Typer< float >
	{
		constexpr static GlES3Type Value = GlES3Type::eFloat;
	};
	template <> struct GlES3Typer< double >
	{
		constexpr static GlES3Type Value = GlES3Type::eDouble;
	};

	//**********************************************************************************************

	template< typename T, uint32_t Count >
	GlES3VecAttribute< T, Count >::GlES3VecAttribute( OpenGlES3 & p_gl, Castor3D::ShaderProgram const & p_program, uint32_t p_stride, Castor::String const & p_attributeName, uint32_t p_divisor )
		: GlES3AttributeBase( p_gl, p_program, p_stride, p_attributeName, GlES3Typer< T >::Value, Count, p_divisor )
	{
	}

	template< typename T, uint32_t Count >
	GlES3VecAttribute< T, Count >::~GlES3VecAttribute()
	{
	}

	template< typename T, uint32_t Count >
	void GlES3VecAttribute< T, Count >::Bind( bool p_bNormalised )
	{
		GetOpenGlES3().EnableVertexAttribArray( m_attributeLocation );

		if ( m_glType == GlES3Type::eFloat
			 || m_glType == GlES3Type::eDouble
			 || m_glType == GlES3Type::eHalfFloat )
		{
			GetOpenGlES3().VertexAttribPointer( m_attributeLocation, m_count, m_glType, p_bNormalised, m_stride, BUFFER_OFFSET( m_offset ) );
		}
		else
		{
			GetOpenGlES3().VertexAttribPointer( m_attributeLocation, m_count, m_glType, m_stride, BUFFER_OFFSET( m_offset ) );
		}

		if ( m_divisor )
		{
			GetOpenGlES3().VertexAttribDivisor( m_attributeLocation, m_divisor );
		}
	}

	//**********************************************************************************************

	template< typename T, uint32_t Columns, uint32_t Rows >
	GlES3MatAttribute< T, Columns, Rows >::GlES3MatAttribute( OpenGlES3 & p_gl, Castor3D::ShaderProgram const & p_program, uint32_t p_stride, Castor::String const & p_attributeName )
		: GlES3AttributeBase( p_gl, p_program, p_stride, p_attributeName, GlES3Typer< T >::Value, Columns, 1 )
	{
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	GlES3MatAttribute< T, Columns, Rows >::~GlES3MatAttribute()
	{
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	void GlES3MatAttribute< T, Columns, Rows >::Bind( bool p_bNormalised )
	{
		uint32_t l_offset = m_offset;
		const uint32_t l_off = Rows * sizeof( T );

		if ( m_glType == GlES3Type::eInt )
		{
			for ( int i = 0; i < Columns; ++i )
			{
				uint32_t l_location = uint32_t( m_attributeLocation + i );
				GetOpenGlES3().EnableVertexAttribArray( l_location );
				GetOpenGlES3().VertexAttribPointer( l_location, int( Rows ), m_glType, int( m_stride ), BUFFER_OFFSET( l_offset ) );
				GetOpenGlES3().VertexAttribDivisor( l_location, m_divisor );
				l_offset += l_off;
			}
		}
		else
		{
			for ( int i = 0; i < Columns; ++i )
			{
				uint32_t l_location = uint32_t( m_attributeLocation + i );
				GetOpenGlES3().EnableVertexAttribArray( l_location );
				GetOpenGlES3().VertexAttribPointer( l_location, int( Rows ), m_glType, p_bNormalised, int( m_stride ), BUFFER_OFFSET( l_offset ) );
				GetOpenGlES3().VertexAttribDivisor( l_location, m_divisor );
				l_offset += l_off;
			}
		}
	}

	//**********************************************************************************************
}
