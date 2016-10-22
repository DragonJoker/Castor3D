namespace GlRender
{
	//**********************************************************************************************

	template< typename T > struct GlTyper;

	template <> struct GlTyper< int >
	{
		enum { Value = eGL_TYPE_INT };
	};
	template <> struct GlTyper< uint32_t >
	{
		enum { Value = eGL_TYPE_UNSIGNED_INT };
	};
	template <> struct GlTyper< short >
	{
		enum { Value = eGL_TYPE_SHORT };
	};
	template <> struct GlTyper< uint16_t >
	{
		enum { Value = eGL_TYPE_UNSIGNED_SHORT };
	};
	template <> struct GlTyper< char >
	{
		enum { Value = eGL_TYPE_BYTE };
	};
	template <> struct GlTyper< uint8_t >
	{
		enum { Value = eGL_TYPE_UNSIGNED_BYTE };
	};
	template <> struct GlTyper< float >
	{
		enum { Value = eGL_TYPE_FLOAT };
	};
	template <> struct GlTyper< double >
	{
		enum { Value = eGL_TYPE_DOUBLE };
	};

	//**********************************************************************************************

	template< typename T, uint32_t Count >
	GlVecAttribute< T, Count >::GlVecAttribute( OpenGl & p_gl, Castor3D::ShaderProgram const & p_program, uint32_t p_stride, Castor::String const & p_attributeName, uint32_t p_id )
		: GlAttributeBase( p_gl, p_program, p_stride, p_attributeName, p_id, eGL_TYPE( GlTyper< T >::Value ), Count, 0 )
	{
	}

	template< typename T, uint32_t Count >
	GlVecAttribute< T, Count >::~GlVecAttribute()
	{
	}

	//**********************************************************************************************

	template< typename T, uint32_t Columns, uint32_t Rows >
	GlMatAttribute< T, Columns, Rows >::GlMatAttribute( OpenGl & p_gl, Castor3D::ShaderProgram const & p_program, uint32_t p_stride, Castor::String const & p_attributeName, uint32_t p_id )
		: GlAttributeBase( p_gl, p_program, p_stride, p_attributeName, p_id, eGL_TYPE( GlTyper< T >::Value ), Columns, 1 )
	{
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	GlMatAttribute< T, Columns, Rows >::~GlMatAttribute()
	{
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	bool GlMatAttribute< T, Columns, Rows >::Bind( bool p_bNormalised )
	{
		bool l_return = true;
		uint32_t l_offset = m_offset;
		const uint32_t l_off = Rows * sizeof( T );

		if ( m_glType == eGL_TYPE_INT )
		{
			for ( int i = 0; i < Columns && l_return; ++i )
			{
				uint32_t l_location = uint32_t( m_attributeLocation + i );
				l_return = GetOpenGl().EnableVertexAttribArray( l_location );
				l_return &= GetOpenGl().VertexAttribPointer( l_location, int( Rows ), m_glType, int( m_stride ), BUFFER_OFFSET( l_offset ) );
				l_return &= GetOpenGl().VertexAttribDivisor( l_location, m_divisor );
				l_offset += l_off;
			}
		}
		else
		{
			for ( int i = 0; i < Columns && l_return; ++i )
			{
				uint32_t l_location = uint32_t( m_attributeLocation + i );
				l_return = GetOpenGl().EnableVertexAttribArray( l_location );
				l_return &= GetOpenGl().VertexAttribPointer( l_location, int( Rows ), m_glType, p_bNormalised, int( m_stride ), BUFFER_OFFSET( l_offset ) );
				l_return &= GetOpenGl().VertexAttribDivisor( l_location, m_divisor );
				l_offset += l_off;
			}
		}

		return l_return;
	}

	//**********************************************************************************************
}
