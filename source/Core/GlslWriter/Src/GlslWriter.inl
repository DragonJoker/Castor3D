namespace glsl
{
	//***********************************************************************************************

	template< typename T >
	struct Outputgetter
	{
	};

	template< typename T >
	inline GlslWriter & operator<<( GlslWriter & p_stream, Outputgetter< T > const & p_in )
	{
		p_stream << Out();
		return p_stream;
	}

	template<>
	inline GlslWriter & operator<<( GlslWriter & p_stream, Outputgetter< Int > const & p_in )
	{
		p_stream << "flat " << Out();
		return p_stream;
	}

	template<>
	inline GlslWriter & operator<<( GlslWriter & p_stream, Outputgetter< UInt > const & p_in )
	{
		p_stream << "flat " << Out();
		return p_stream;
	}

	//***********************************************************************************************

	template< typename T >
	struct Inputgetter
	{
	};

	template< typename T >
	inline GlslWriter & operator<<( GlslWriter & p_stream, Inputgetter< T > const & p_in )
	{
		p_stream << In();
		return p_stream;
	}

	template<>
	inline GlslWriter & operator<<( GlslWriter & p_stream, Inputgetter< Int > const & p_in )
	{
		p_stream << "flat " << In();
		return p_stream;
	}

	template<>
	inline GlslWriter & operator<<( GlslWriter & p_stream, Inputgetter< UInt > const & p_in )
	{
		p_stream << "flat " << In();
		return p_stream;
	}


	//***********************************************************************************************

	template< typename T >
	void writeAssign( GlslWriter * p_writer, Type const & p_lhs, T const & p_rhs )
	{
		p_writer->writeAssign( p_lhs, p_rhs );
	}

	template< typename T1, typename T2 >
	void writeAssign( GlslWriter * p_writer, Optional< T1 > const & p_lhs, T2 const & p_rhs )
	{
		p_writer->writeAssign( p_lhs, p_rhs );
	}

	//***********************************************************************************************

	template< typename T >
	void GlslWriter::writeAssign( Type const & p_lhs, Optional< T > const & p_rhs )
	{
		if ( p_rhs.isEnabled() )
		{
			m_stream << castor::String( p_lhs ) << cuT( " = " ) << castor::String( p_rhs ) << cuT( ";" ) << std::endl;
		}
	}

	template< typename RetType, typename FuncType, typename ... Params >
	inline Function< RetType, Params... > GlslWriter::implementFunction( castor::String const & p_name, FuncType p_function, Params && ... p_params )
	{
		m_stream << std::endl;
		writeFunctionHeader< RetType >( *this, p_name, p_params... );
		{
			IndentBlock block( *this );
			p_function( std::forward < Params && > ( p_params )... );
		}
		m_stream << std::endl;
		return Function< RetType, Params... >{ this, p_name };
	}

	template< typename RetType >
	inline void GlslWriter::returnStmt( RetType const & p_return )
	{
		m_stream << cuT( "return " ) << castor::String( p_return ) << cuT( ";" ) << std::endl;
	}

	template< typename ExprType >
	ExprType GlslWriter::paren( ExprType const p_expr )
	{
		ExprType result( this );
		result.m_value << cuT( "( " ) << castor::String( p_expr ) << cuT( " )" );
		return result;
	}

	template< typename ExprType >
	ExprType GlslWriter::ternary( Type const & p_condition, ExprType const & p_left, ExprType const & p_right )
	{
		ExprType result( this );
		result.m_value << cuT( "( ( " ) << toString( p_condition ) << cuT( " ) ? " ) << toString( p_left ) << cuT( " : " ) << toString( p_right ) << cuT( " )" );
		return result;
	}

	template< typename T >
	inline T GlslWriter::cast( Type const & p_from )
	{
		T result;
		result.m_value << castor::string::trim( result.m_type ) << cuT( "( " ) << castor::String( p_from ) << cuT( " )" );
		return result;
	}

	template< typename T >
	inline T GlslWriter::declAttribute( castor::String const & p_name )
	{
		using type = typename type_of< T >::type;
		registerInput( p_name, name_of< T >::value );
		*this << Attribute() << type().m_type << p_name << cuT( ";" ) << Endl();

		if ( std::is_same< T, Mat4 >::value )
		{
			m_attributeIndex += 3;
		}

		return T( this, p_name );
	}

	template< typename T >
	inline T GlslWriter::declOutput( castor::String const & p_name )
	{
		using type = typename type_of< T >::type;
		registerOutput( p_name, name_of< T >::value );
		*this << Outputgetter< T >() << type().m_type << p_name << cuT( ";" ) << Endl();
		return T( this, p_name );
	}

	template< typename T >
	inline T GlslWriter::declInput( castor::String const & p_name )
	{
		using type = typename type_of< T >::type;
		registerInput( p_name, name_of< T >::value );
		*this << Inputgetter< T >() << type().m_type << p_name << cuT( ";" ) << Endl();
		return T( this, p_name );
	}

	template< typename T >
	inline T GlslWriter::declLocale( castor::String const & p_name )
	{
		using type = typename type_of< T >::type;
		registerName( p_name, name_of< T >::value );
		*this << type().m_type << p_name << cuT( ";" ) << Endl();
		return T( this, p_name );
	}

	template< typename T >
	inline T GlslWriter::declLocale( castor::String const & p_name, T const & p_rhs )
	{
		using type = typename type_of< T >::type;
		registerName( p_name, name_of< T >::value );
		m_stream << type().m_type << p_name << cuT( " = " ) << castor::String( p_rhs ) << cuT( ";" ) << std::endl;
		T result( this, p_name );
		return result;
	}

	template< typename T >
	inline T GlslWriter::declBuiltin( castor::String const & p_name )
	{
		registerName( p_name, name_of< T >::value );
		return T( this, p_name );
	}

	template< typename T >
	inline T GlslWriter::getBuiltin( castor::String const & p_name )
	{
		checkNameExists( p_name, name_of< T >::value );
		return T( this, p_name );
	}

	template< typename T >
	inline T GlslWriter::declUniform( castor::String const & p_name )
	{
		using type = typename type_of< T >::type;
		registerUniform( p_name, name_of< T >::value );
		*this << Uniform() << type().m_type << p_name << cuT( ";" ) << Endl();
		return T( this, p_name );
	}

	template< typename T >
	inline T GlslWriter::declUniform( castor::String const & p_name, T const & p_rhs )
	{
		using type = typename type_of< T >::type;
		registerUniform( p_name, name_of< T >::value );
		*this << Uniform() << type().m_type << p_name << cuT( " = " );
		m_stream << castor::String( p_rhs ) << cuT( ";" ) << std::endl;
		return T( this, p_name );
	}

	template< typename T >
	inline T GlslWriter::declFragData( castor::String const & p_name, uint32_t p_index )
	{
		using type = typename type_of< T >::type;
		registerOutput( p_name, name_of< T >::value );
		castor::String name;

		if ( m_keywords->hasNamedFragData() )
		{
			*this << Layout { int( p_index ) } << Outputgetter< T >() << type().m_type << p_name << cuT( ";" ) << Endl();
			name = p_name;
		}
		else
		{
			name = m_keywords->getFragData( p_index );
		}

		return T( this, name );
	}

	template< typename T >
	inline Array< T > GlslWriter::declAttribute( castor::String const & p_name, uint32_t p_dimension )
	{
		using type = typename type_of< T >::type;
		registerInput( p_name, name_of< T >::value );
		*this << Attribute() << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();

		if ( std::is_same< T, Mat4 >::value )
		{
			m_attributeIndex += 3;
		}

		return Array< T >( this, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::declOutput( castor::String const & p_name, uint32_t p_dimension )
	{
		using type = typename type_of< T >::type;
		registerOutput( p_name, name_of< T >::value );
		*this << Outputgetter< T >() << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
		return Array< T >( this, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::declOutputArray( castor::String const & p_name )
	{
		using type = typename type_of< T >::type;
		registerOutput( p_name, name_of< T >::value );
		*this << Outputgetter< T >() << type().m_type << p_name << cuT( "[];" ) << Endl();
		return Array< T >( this, p_name, 32u );
	}

	template< typename T >
	inline Array< T > GlslWriter::declInput( castor::String const & p_name, uint32_t p_dimension )
	{
		using type = typename type_of< T >::type;
		registerInput( p_name, name_of< T >::value );
		*this << Inputgetter< T >() << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
		return Array< T >( this, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::declInputArray( castor::String const & p_name )
	{
		using type = typename type_of< T >::type;
		registerInput( p_name, name_of< T >::value );
		*this << Inputgetter< T >() << type().m_type << p_name << cuT( "[];" ) << Endl();
		return Array< T >( this, p_name, 32u );
	}

	template< typename T >
	inline Array< T > GlslWriter::declLocale( castor::String const & p_name, uint32_t p_dimension )
	{
		using type = typename type_of< T >::type;
		registerName( p_name, name_of< T >::value );
		*this << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( ";" ) << Endl();
		return Array< T >( this, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::declLocale( castor::String const & p_name, uint32_t p_dimension, T const & p_rhs )
	{
		using type = typename type_of< T >::type;
		registerName( p_name, name_of< T >::value );
		m_stream << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( " = " ) << castor::String( p_rhs ) << cuT( ";" ) << std::endl;
		return Array< T >( this, p_name );
	}

	template< typename T >
	inline Array< T > GlslWriter::declBuiltin( castor::String const & p_name, uint32_t p_dimension )
	{
		registerName( p_name, name_of< T >::value );
		return Array< T >( this, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::declBuiltinArray( castor::String const & p_name )
	{
		registerName( p_name, name_of< T >::value );
		return Array< T >( this, p_name, 0xFFFFFFFF );
	}

	template< typename T >
	inline Array< T > GlslWriter::getBuiltin( castor::String const & p_name, uint32_t p_dimension )
	{
		checkNameExists( p_name, name_of< T >::value );
		return Array< T >( this, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::getBuiltinArray( castor::String const & p_name )
	{
		checkNameExists( p_name, name_of< T >::value );
		return Array< T >( this, p_name, 0xFFFFFFFF );
	}

	template< typename T >
	inline Array< T > GlslWriter::declUniform( castor::String const & p_name, uint32_t p_dimension )
	{
		using type = typename type_of< T >::type;
		registerUniform( p_name, name_of< T >::value );
		*this << Uniform() << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
		return Array< T >( this, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::declUniformArray( castor::String const & p_name )
	{
		using type = typename type_of< T >::type;
		registerUniform( p_name, name_of< T >::value );
		*this << Uniform() << type().m_type << p_name << cuT( "[];" ) << Endl();
		return Array< T >( this, p_name, 0xFFFFFFFF );
	}

	template< typename T >
	inline Array< T > GlslWriter::declUniform( castor::String const & p_name, uint32_t p_dimension, std::vector< T > const & p_rhs )
	{
		using type = typename type_of< T >::type;
		if ( p_rhs.size() != p_dimension )
		{
			CASTOR_EXCEPTION( "Given parameters count doesn't match the array dimensions" );
		}
		
		registerUniform( p_name, name_of< T >::value );
		*this << Uniform() << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "] = " ) << type().m_type << cuT( "[]( " );
		castor::String sep;

		for ( auto const & value : p_rhs )
		{
			*this << sep << castor::String( value ) << Endl();
			sep = cuT( "\t, " );
		}

		*this << cuT( ");" ) << Endl();
		return Array< T >( this, p_name, p_dimension );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declAttribute( castor::String const & p_name, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		registerInput( p_name, name_of< T >::value );

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
	inline Optional< T > GlslWriter::declOutput( castor::String const & p_name, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		registerOutput( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << Outputgetter< T >() << type().m_type << p_name << cuT( ";" ) << Endl();
		}

		return Optional< T >( this, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declInput( castor::String const & p_name, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		registerInput( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << Inputgetter< T >() << type().m_type << p_name << cuT( ";" ) << Endl();
		}

		return Optional< T >( this, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declLocale( castor::String const & p_name, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		registerName( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << type().m_type << p_name << cuT( ";" ) << Endl();
		}

		return Optional< T >( this, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declLocale( castor::String const & p_name, bool p_enabled, T const & p_rhs )
	{
		using type = typename type_of< T >::type;
		registerName( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			m_stream << type().m_type << p_name << cuT( " = " ) << castor::String( p_rhs ) << cuT( ";" ) << std::endl;
		}

		return Optional< T >( this, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declBuiltin( castor::String const & p_name, bool p_enabled )
	{
		registerName( p_name, name_of< T >::value );
		return Optional< T >( this, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::getBuiltin( castor::String const & p_name, bool p_enabled )
	{
		checkNameExists( p_name, name_of< T >::value );
		return Optional< T >( this, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declUniform( castor::String const & p_name, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		registerUniform( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << Uniform() << type().m_type << p_name << cuT( ";" ) << Endl();
		}

		return Optional< T >( this, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declUniform( castor::String const & p_name, bool p_enabled, T const & p_rhs )
	{
		using type = typename type_of< T >::type;
		registerUniform( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << Uniform() << type().m_type << p_name << cuT( " = " );
			m_stream << castor::String( p_rhs ) << cuT( ";" ) << std::endl;
		}

		return Optional< T >( this, p_name, p_enabled );
	}
	
	template< typename T >
	inline Optional< Array< T > > GlslWriter::declAttribute( castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		registerAttribute( p_name, name_of< T >::value );

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
	inline Optional< Array< T > > GlslWriter::declOutput( castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		registerOutput( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << Outputgetter< T >() << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( ";" ) << Endl();
		}

		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}
	
	template< typename T >
	inline Optional< Array< T > > GlslWriter::declOutputArray( castor::String const & p_name, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		registerOutput( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << Outputgetter< T >() << type().m_type << p_name << cuT( "[]" ) << cuT( ";" ) << Endl();
		}

		return Optional< Array< T > >( this, p_name, 0xFFFFFFFF, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declInput( castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		registerInput( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << Inputgetter< T >() << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( ";" ) << Endl();
		}

		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declInputArray( castor::String const & p_name, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		registerInput( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << Inputgetter< T >() << type().m_type << p_name << cuT( "[]" ) << cuT( ";" ) << Endl();
		}

		return Optional< Array< T > >( this, p_name, 0xFFFFFFFF, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declLocale( castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		registerName( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( ";" ) << Endl();
		}

		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declLocale( castor::String const & p_name, uint32_t p_dimension, bool p_enabled, T const & p_rhs )
	{
		using type = typename type_of< T >::type;
		registerName( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			m_stream << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( " = " ) << castor::String( p_rhs ) << cuT( ";" ) << std::endl;
		}

		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declBuiltin( castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		registerName( p_name, name_of< T >::value );
		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declBuiltinArray( castor::String const & p_name, bool p_enabled )
	{
		registerName( p_name, name_of< T >::value );
		return Optional< Array< T > >( this, p_name, 0xFFFFFFFF, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::getBuiltin( castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		checkNameExists( p_name, name_of< T >::value );
		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::getBuiltinArray( castor::String const & p_name, bool p_enabled )
	{
		checkNameExists( p_name, name_of< T >::value );
		return Optional< Array< T > >( this, p_name, 0xFFFFFFFF, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declUniform( castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		registerUniform( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << Uniform() << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
		}

		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declUniformArray( castor::String const & p_name, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		registerUniform( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << Uniform() << type().m_type << p_name << cuT( "[];" ) << Endl();
		}

		return Optional< Array< T > >( this, p_name, 0xFFFFFFFF, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declUniform( castor::String const & p_name, uint32_t p_dimension, bool p_enabled, std::vector< T > const & p_rhs )
	{
		using type = typename type_of< T >::type;
		if ( p_rhs.size() != p_dimension )
		{
			CASTOR_EXCEPTION( "Given parameters count doesn't match the array dimensions" );
		}

		registerUniform( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			*this << Uniform() << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "] = " ) << type().m_type << cuT( "[]( " );
			castor::String sep;

			for ( auto const & value : p_rhs )
			{
				*this << sep << castor::String( value ) << Endl();
				sep = cuT( "\t, " );
			}

			*this << cuT( ");" ) << Endl();
		}

		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}
}
