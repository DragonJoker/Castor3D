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
	GlAttribute< T, Count >::GlAttribute( OpenGl & p_gl, Castor3D::ShaderProgram const & p_program, Castor::String const & p_attributeName )
		: GlAttributeBase( p_gl, p_program, p_attributeName, eGL_TYPE( GlTyper< T >::Value ), Count, 0 )
	{
	}

	template< typename T, uint32_t Count >
	GlAttribute< T, Count >::~GlAttribute()
	{
	}

	//**********************************************************************************************

	template< typename T, uint32_t Columns, uint32_t Rows >
	GlMatAttribute< T, Columns, Rows >::GlMatAttribute( OpenGl & p_gl, Castor3D::ShaderProgram const & p_program, Castor::String const & p_attributeName )
		: GlAttributeBase( p_gl, p_program, p_attributeName, eGL_TYPE( GlTyper< T >::Value ), Columns, 1 )
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

		if ( m_glType == eGL_TYPE_INT )
		{
			for ( int i = 0; i < Rows && l_return; ++i )
			{
				l_return = GetOpenGl().EnableVertexAttribArray( m_attributeLocation + i );
				l_return &= GetOpenGl().VertexAttribPointer( m_attributeLocation + i, Columns, m_glType, m_stride, BUFFER_OFFSET( m_offset + ( i * Columns * sizeof( T ) ) ) );
				l_return &= GetOpenGl().VertexAttribDivisor( m_attributeLocation + i, 1 );
			}
		}
		else
		{
			for ( int i = 0; i < Rows && l_return; ++i )
			{
				l_return = GetOpenGl().EnableVertexAttribArray( m_attributeLocation + i );
				l_return &= GetOpenGl().VertexAttribPointer( m_attributeLocation + i, Columns, m_glType, p_bNormalised, m_stride, BUFFER_OFFSET( m_offset + ( i * Columns * sizeof( T ) ) ) );
				l_return &= GetOpenGl().VertexAttribDivisor( m_attributeLocation + i, 1 );
			}
		}

		return l_return;
	}

	//**********************************************************************************************
}
