namespace GLSL
{
	//***********************************************************************************************

	template< typename T >
	struct OutputGetter
	{
	};

	template< typename T >
	inline GlslWriter & operator<<( GlslWriter & p_stream, OutputGetter< T > const & p_in )
	{
		p_stream << Out();
		return p_stream;
	}

	template<>
	inline GlslWriter & operator<<( GlslWriter & p_stream, OutputGetter< Int > const & p_in )
	{
		p_stream << "flat " << Out();
		return p_stream;
	}

	template<>
	inline GlslWriter & operator<<( GlslWriter & p_stream, OutputGetter< UInt > const & p_in )
	{
		p_stream << "flat " << Out();
		return p_stream;
	}

	//***********************************************************************************************

	template< typename T >
	struct InputGetter
	{
	};

	template< typename T >
	inline GlslWriter & operator<<( GlslWriter & p_stream, InputGetter< T > const & p_in )
	{
		p_stream << In();
		return p_stream;
	}

	template<>
	inline GlslWriter & operator<<( GlslWriter & p_stream, InputGetter< Int > const & p_in )
	{
		p_stream << "flat " << In();
		return p_stream;
	}

	template<>
	inline GlslWriter & operator<<( GlslWriter & p_stream, InputGetter< UInt > const & p_in )
	{
		p_stream << "flat " << In();
		return p_stream;
	}


	//***********************************************************************************************

	template< typename T >
	void WriteAssign( GlslWriter * p_writer, Type const & p_lhs, T const & p_rhs )
	{
		p_writer->WriteAssign( p_lhs, p_rhs );
	}

	template< typename T1, typename T2 >
	void WriteAssign( GlslWriter * p_writer, Optional< T1 > const & p_lhs, T2 const & p_rhs )
	{
		p_writer->WriteAssign( p_lhs, p_rhs );
	}

	//***********************************************************************************************

	template< typename T >
	void GlslWriter::WriteAssign( Type const & p_lhs, Optional< T > const & p_rhs )
	{
		if ( p_rhs.IsEnabled() )
		{
			m_stream << Castor::String( p_lhs ) << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << std::endl;
		}
	}

	template< typename RetType, typename FuncType, typename ... Params >
	inline Function< RetType, Params... > GlslWriter::ImplementFunction( Castor::String const & p_name, FuncType p_function, Params && ... p_params )
	{
		m_stream << std::endl;
		WriteFunctionHeader< RetType >( *this, p_name, p_params... );
		{
			IndentBlock block( *this );
			p_function( std::forward < Params && > ( p_params )... );
		}
		m_stream << std::endl;
		return Function< RetType, Params... >{ this, p_name };
	}

	template< typename RetType >
	inline void GlslWriter::Return( RetType const & p_return )
	{
		m_stream << cuT( "return " ) << Castor::String( p_return ) << cuT( ";" ) << std::endl;
	}

	template< typename ExprType >
	ExprType GlslWriter::Paren( ExprType const p_expr )
	{
		ExprType result( this );
		result.m_value << cuT( "( " ) << Castor::String( p_expr ) << cuT( " )" );
		return result;
	}

	template< typename ExprType >
	ExprType GlslWriter::Ternary( Type const & p_condition, ExprType const & p_left, ExprType const & p_right )
	{
		ExprType result( this );
		result.m_value << cuT( "( ( " ) << ToString( p_condition ) << cuT( " ) ? " ) << ToString( p_left ) << cuT( " : " ) << ToString( p_right ) << cuT( " )" );
		return result;
	}

	template< typename T >
	inline T GlslWriter::Cast( Type const & p_from )
	{
		T result;
		result.m_value << Castor::string::trim( result.m_type ) << cuT( "( " ) << Castor::String( p_from ) << cuT( " )" );
		return result;
	}

	template< typename T >
	inline T GlslWriter::DeclAttribute( Castor::String const & p_name )
	{
		using type = typename type_of< T >::type;
		RegisterInput( p_name, name_of< T >::value );
		*this << Attribute() << type().m_type << p_name << cuT( ";" ) << Endl();

		if ( std::is_same< T, Mat4 >::value )
		{
			m_attributeIndex += 3;
		}

		return T( this, p_name );
	}

	template< typename T >
	inline T GlslWriter::DeclOutput( Castor::String const & p_name )
	{
		using type = typename type_of< T >::type;
		RegisterOutput( p_name, name_of< T >::value );
		*this << OutputGetter< T >() << type().m_type << p_name << cuT( ";" ) << Endl();
		return T( this, p_name );
	}

	template< typename T >
	inline T GlslWriter::DeclInput( Castor::String const & p_name )
	{
		using type = typename type_of< T >::type;
		RegisterInput( p_name, name_of< T >::value );
		*this << InputGetter< T >() << type().m_type << p_name << cuT( ";" ) << Endl();
		return T( this, p_name );
	}

	template< typename T >
	inline T GlslWriter::DeclLocale( Castor::String const & p_name )
	{
		using type = typename type_of< T >::type;
		RegisterName( p_name, name_of< T >::value );
		*this << type().m_type << p_name << cuT( ";" ) << Endl();
		return T( this, p_name );
	}

	template< typename T >
	inline T GlslWriter::DeclLocale( Castor::String const & p_name, T const & p_rhs )
	{
		using type = typename type_of< T >::type;
		RegisterName( p_name, name_of< T >::value );
		m_stream << type().m_type << p_name << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << std::endl;
		T result( this, p_name );
		return result;
	}

	template< typename T >
	inline T GlslWriter::DeclBuiltin( Castor::String const & p_name )
	{
		RegisterName( p_name, name_of< T >::value );
		return T( this, p_name );
	}

	template< typename T >
	inline T GlslWriter::GetBuiltin( Castor::String const & p_name )
	{
		CheckNameExists( p_name, name_of< T >::value );
		return T( this, p_name );
	}

	template< typename T >
	inline T GlslWriter::DeclUniform( Castor::String const & p_name )
	{
		using type = typename type_of< T >::type;
		RegisterUniform( p_name, name_of< T >::value );
		*this << Uniform() << type().m_type << p_name << cuT( ";" ) << Endl();
		return T( this, p_name );
	}

	template< typename T >
	inline T GlslWriter::DeclUniform( Castor::String const & p_name, T const & p_rhs )
	{
		using type = typename type_of< T >::type;
		RegisterUniform( p_name, name_of< T >::value );
		*this << Uniform() << type().m_type << p_name << cuT( " = " );
		m_stream << Castor::String( p_rhs ) << cuT( ";" ) << std::endl;
		return T( this, p_name );
	}

	template< typename T >
	inline T GlslWriter::DeclFragData( Castor::String const & p_name, uint32_t p_index )
	{
		using type = typename type_of< T >::type;
		RegisterOutput( p_name, name_of< T >::value );
		Castor::String name;

		if ( m_keywords->HasNamedFragData() )
		{
			*this << Layout { int( p_index ) } << OutputGetter< T >() << type().m_type << p_name << cuT( ";" ) << Endl();
			name = p_name;
		}
		else
		{
			name = m_keywords->GetFragData( p_index );
		}

		return T( this, name );
	}

	template< typename T >
	inline Array< T > GlslWriter::DeclAttribute( Castor::String const & p_name, uint32_t p_dimension )
	{
		using type = typename type_of< T >::type;
		RegisterInput( p_name, name_of< T >::value );
		*this << Attribute() << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();

		if ( std::is_same< T, Mat4 >::value )
		{
			m_attributeIndex += 3;
		}

		return Array< T >( this, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::DeclOutput( Castor::String const & p_name, uint32_t p_dimension )
	{
		using type = typename type_of< T >::type;
		RegisterOutput( p_name, name_of< T >::value );
		*this << OutputGetter< T >() << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
		return Array< T >( this, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::DeclOutputArray( Castor::String const & p_name )
	{
		using type = typename type_of< T >::type;
		RegisterOutput( p_name, name_of< T >::value );
		*this << OutputGetter< T >() << type().m_type << p_name << cuT( "[];" ) << Endl();
		return Array< T >( this, p_name, 32u );
	}

	template< typename T >
	inline Array< T > GlslWriter::DeclInput( Castor::String const & p_name, uint32_t p_dimension )
	{
		using type = typename type_of< T >::type;
		RegisterInput( p_name, name_of< T >::value );
		*this << InputGetter< T >() << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
		return Array< T >( this, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::DeclInputArray( Castor::String const & p_name )
	{
		using type = typename type_of< T >::type;
		RegisterInput( p_name, name_of< T >::value );
		*this << InputGetter< T >() << type().m_type << p_name << cuT( "[];" ) << Endl();
		return Array< T >( this, p_name, 32u );
	}

	template< typename T >
	inline Array< T > GlslWriter::DeclLocale( Castor::String const & p_name, uint32_t p_dimension )
	{
		using type = typename type_of< T >::type;
		RegisterName( p_name, name_of< T >::value );
		*this << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( ";" ) << Endl();
		return Array< T >( this, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::DeclLocale( Castor::String const & p_name, uint32_t p_dimension, T const & p_rhs )
	{
		using type = typename type_of< T >::type;
		RegisterName( p_name, name_of< T >::value );
		m_stream << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << std::endl;
		return Array< T >( this, p_name );
	}

	template< typename T >
	inline Array< T > GlslWriter::DeclBuiltin( Castor::String const & p_name, uint32_t p_dimension )
	{
		RegisterName( p_name, name_of< T >::value );
		return Array< T >( this, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::DeclBuiltinArray( Castor::String const & p_name )
	{
		RegisterName( p_name, name_of< T >::value );
		return Array< T >( this, p_name, 0xFFFFFFFF );
	}

	template< typename T >
	inline Array< T > GlslWriter::GetBuiltin( Castor::String const & p_name, uint32_t p_dimension )
	{
		CheckNameExists( p_name, name_of< T >::value );
		return Array< T >( this, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::GetBuiltinArray( Castor::String const & p_name )
	{
		CheckNameExists( p_name, name_of< T >::value );
		return Array< T >( this, p_name, 0xFFFFFFFF );
	}

	template< typename T >
	inline Array< T > GlslWriter::DeclUniform( Castor::String const & p_name, uint32_t p_dimension )
	{
		using type = typename type_of< T >::type;
		RegisterUniform( p_name, name_of< T >::value );
		*this << Uniform() << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
		return Array< T >( this, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::DeclUniformArray( Castor::String const & p_name )
	{
		using type = typename type_of< T >::type;
		RegisterUniform( p_name, name_of< T >::value );
		*this << Uniform() << type().m_type << p_name << cuT( "[];" ) << Endl();
		return Array< T >( this, p_name, 0xFFFFFFFF );
	}

	template< typename T >
	inline Array< T > GlslWriter::DeclUniform( Castor::String const & p_name, uint32_t p_dimension, std::vector< T > const & p_rhs )
	{
		using type = typename type_of< T >::type;
		if ( p_rhs.size() != p_dimension )
		{
			CASTOR_EXCEPTION( "Given parameters count doesn't match the array dimensions" );
		}
		
		RegisterUniform( p_name, name_of< T >::value );
		*this << Uniform() << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "] = " ) << type().m_type << cuT( "[]( " );
		Castor::String sep;

		for ( auto const & value : p_rhs )
		{
			*this << sep << Castor::String( value ) << Endl();
			sep = cuT( "\t, " );
		}

		*this << cuT( ");" ) << Endl();
		return Array< T >( this, p_name, p_dimension );
	}

	template< typename T >
	inline Optional< T > GlslWriter::DeclAttribute( Castor::String const & p_name, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		RegisterInput( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << Attribute() << type().m_type << p_name << cuT( ";" ) << Endl();

			if ( std::is_same< T, Mat4 >::value )
			{
				m_attributeIndex += 3;
			}
		}

		return Optional< T >( this, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::DeclOutput( Castor::String const & p_name, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		RegisterOutput( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << OutputGetter< T >() << type().m_type << p_name << cuT( ";" ) << Endl();
		}

		return Optional< T >( this, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::DeclInput( Castor::String const & p_name, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		RegisterInput( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << InputGetter< T >() << type().m_type << p_name << cuT( ";" ) << Endl();
		}

		return Optional< T >( this, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::DeclLocale( Castor::String const & p_name, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		RegisterName( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << type().m_type << p_name << cuT( ";" ) << Endl();
		}

		return Optional< T >( this, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::DeclLocale( Castor::String const & p_name, bool p_enabled, T const & p_rhs )
	{
		using type = typename type_of< T >::type;
		RegisterName( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			m_stream << type().m_type << p_name << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << std::endl;
		}

		return Optional< T >( this, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::DeclBuiltin( Castor::String const & p_name, bool p_enabled )
	{
		RegisterName( p_name, name_of< T >::value );
		return Optional< T >( this, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::GetBuiltin( Castor::String const & p_name, bool p_enabled )
	{
		CheckNameExists( p_name, name_of< T >::value );
		return Optional< T >( this, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::DeclUniform( Castor::String const & p_name, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		RegisterUniform( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << Uniform() << type().m_type << p_name << cuT( ";" ) << Endl();
		}

		return Optional< T >( this, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::DeclUniform( Castor::String const & p_name, bool p_enabled, T const & p_rhs )
	{
		using type = typename type_of< T >::type;
		RegisterUniform( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << Uniform() << type().m_type << p_name << cuT( " = " );
			m_stream << Castor::String( p_rhs ) << cuT( ";" ) << std::endl;
		}

		return Optional< T >( this, p_name, p_enabled );
	}
	
	template< typename T >
	inline Optional< Array< T > > GlslWriter::DeclAttribute( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		RegisterAttribute( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << Attribute() << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( ";" ) << Endl();

			if ( std::is_same< T, Mat4 >::value )
			{
				m_attributeIndex += 3;
			}
		}

		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}
	
	template< typename T >
	inline Optional< Array< T > > GlslWriter::DeclOutput( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		RegisterOutput( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << OutputGetter< T >() << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( ";" ) << Endl();
		}

		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}
	
	template< typename T >
	inline Optional< Array< T > > GlslWriter::DeclOutputArray( Castor::String const & p_name, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		RegisterOutput( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << OutputGetter< T >() << type().m_type << p_name << cuT( "[]" ) << cuT( ";" ) << Endl();
		}

		return Optional< Array< T > >( this, p_name, 0xFFFFFFFF, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::DeclInput( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		RegisterInput( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << InputGetter< T >() << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( ";" ) << Endl();
		}

		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::DeclInputArray( Castor::String const & p_name, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		RegisterInput( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << InputGetter< T >() << type().m_type << p_name << cuT( "[]" ) << cuT( ";" ) << Endl();
		}

		return Optional< Array< T > >( this, p_name, 0xFFFFFFFF, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::DeclLocale( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		RegisterName( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( ";" ) << Endl();
		}

		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::DeclLocale( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled, T const & p_rhs )
	{
		using type = typename type_of< T >::type;
		RegisterName( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			m_stream << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << std::endl;
		}

		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::DeclBuiltin( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		RegisterName( p_name, name_of< T >::value );
		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::DeclBuiltinArray( Castor::String const & p_name, bool p_enabled )
	{
		RegisterName( p_name, name_of< T >::value );
		return Optional< Array< T > >( this, p_name, 0xFFFFFFFF, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::GetBuiltin( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		CheckNameExists( p_name, name_of< T >::value );
		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::GetBuiltinArray( Castor::String const & p_name, bool p_enabled )
	{
		CheckNameExists( p_name, name_of< T >::value );
		return Optional< Array< T > >( this, p_name, 0xFFFFFFFF, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::DeclUniform( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		RegisterUniform( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << Uniform() << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
		}

		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::DeclUniformArray( Castor::String const & p_name, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		RegisterUniform( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << Uniform() << type().m_type << p_name << cuT( "[];" ) << Endl();
		}

		return Optional< Array< T > >( this, p_name, 0xFFFFFFFF, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::DeclUniform( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled, std::vector< T > const & p_rhs )
	{
		using type = typename type_of< T >::type;
		if ( p_rhs.size() != p_dimension )
		{
			CASTOR_EXCEPTION( "Given parameters count doesn't match the array dimensions" );
		}

		RegisterUniform( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << Uniform() << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "] = " ) << type().m_type << cuT( "[]( " );
			Castor::String sep;

			for ( auto const & value : p_rhs )
			{
				*this << sep << Castor::String( value ) << Endl();
				sep = cuT( "\t, " );
			}

			*this << cuT( ");" ) << Endl();
		}

		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}
}
