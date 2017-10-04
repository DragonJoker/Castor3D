namespace glsl
{
	//***********************************************************************************************

	template< typename T >
	struct Outputgetter
	{
	};

	template< typename T >
	inline GlslWriter & operator<<( GlslWriter & stream, Outputgetter< T > const & in )
	{
		stream << Out();
		return stream;
	}

	template<>
	inline GlslWriter & operator<<( GlslWriter & stream, Outputgetter< Int > const & in )
	{
		stream << "flat " << Out();
		return stream;
	}

	template<>
	inline GlslWriter & operator<<( GlslWriter & stream, Outputgetter< UInt > const & in )
	{
		stream << "flat " << Out();
		return stream;
	}

	//***********************************************************************************************

	template< typename T >
	struct Inputgetter
	{
	};

	template< typename T >
	inline GlslWriter & operator<<( GlslWriter & stream, Inputgetter< T > const & in )
	{
		stream << In();
		return stream;
	}

	template<>
	inline GlslWriter & operator<<( GlslWriter & stream, Inputgetter< Int > const & in )
	{
		stream << "flat " << In();
		return stream;
	}

	template<>
	inline GlslWriter & operator<<( GlslWriter & stream, Inputgetter< UInt > const & in )
	{
		stream << "flat " << In();
		return stream;
	}


	//***********************************************************************************************

	template< typename T >
	void writeAssign( GlslWriter * writer, Type const & lhs, T const & rhs )
	{
		writer->writeAssign( lhs, rhs );
	}

	template< typename T1, typename T2 >
	void writeAssign( GlslWriter * writer, Optional< T1 > const & lhs, T2 const & rhs )
	{
		writer->writeAssign( lhs, rhs );
	}

	//***********************************************************************************************

	template< typename T >
	void GlslWriter::writeAssign( Type const & lhs, Optional< T > const & rhs )
	{
		if ( rhs.isEnabled() )
		{
			m_stream << castor::String( lhs ) << cuT( " = " ) << castor::String( rhs ) << cuT( ";" ) << std::endl;
		}
	}

	template< typename RetType, typename FuncType, typename ... Params >
	inline Function< RetType, Params... > GlslWriter::implementFunction( castor::String const & name
		, FuncType function
		, Params && ... params )
	{
		m_stream << std::endl;
		writeFunctionHeader< RetType >( *this, name, params... );
		{
			IndentBlock block( *this );
			function( std::forward < Params && > ( params )... );
		}
		m_stream << std::endl;
		return Function< RetType, Params... >{ this, name };
	}

	template< typename RetType >
	inline void GlslWriter::returnStmt( RetType const & result )
	{
		m_stream << cuT( "return " ) << castor::String( result ) << cuT( ";" ) << std::endl;
	}

	template< typename ExprType >
	ExprType GlslWriter::paren( ExprType const expr )
	{
		ExprType result( this );
		result.m_value << cuT( "( " ) << castor::String( expr ) << cuT( " )" );
		return result;
	}

	template< typename ExprType >
	ExprType GlslWriter::ternary( Type const & condition
		, ExprType const & left
		, ExprType const & right )
	{
		ExprType result( this );
		result.m_value << cuT( "( ( " ) << toString( condition ) << cuT( " ) " )
			<< cuT( "? " ) << toString( left ) << cuT( " " )
			<< cuT( ": " ) << toString( right ) << cuT( " )" );
		return result;
	}

	template< typename T >
	inline T GlslWriter::cast( Type const & from )
	{
		T result;
		result.m_value << castor::string::trim( result.m_type ) << cuT( "( " ) << castor::String( from ) << cuT( " )" );
		return result;
	}

	template< typename T >
	inline T GlslWriter::declConstant( castor::String const & name
		, T const & rhs )
	{
		using Type = typename TypeOf< T >::Type;
		registerConstant( name, TypeTraits< Type >::TypeEnum );
		*this << "#define " << name << " ";
		m_stream << castor::String( rhs ) << std::endl;
		return T( this, name );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declConstant( castor::String const & name
		, T const & rhs
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerConstant( name, TypeTraits< Type >::TypeEnum );

		if ( enabled )
		{
			*this << "#define " << name << " ";
			m_stream << castor::String( rhs ) << std::endl;
		}

		return Optional< T >( this, name, enabled );
	}

	template< typename T >
	inline T GlslWriter::declSampler( castor::String const & name
		, uint32_t binding )
	{
		using Type = typename TypeOf< T >::Type;
		registerSampler( name, TypeTraits< Type >::TypeEnum, binding, 1u );
		*this << "layout( binding = " << binding << " ) uniform " << TypeTraits< Type >::Name << " " << name << ";" << endl;
		return T( this, binding, name );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declSampler( castor::String const & name
		, uint32_t binding
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerSampler( name, TypeTraits< Type >::TypeEnum, binding, 1u );

		if ( enabled )
		{
			*this << "layout( binding = " << toString( binding ) << " ) uniform " << TypeTraits< Type >::Name << " " << name << ";" << endl;
		}

		return Optional< T >( this, binding, name, enabled );
	}

	template< typename T >
	inline Array< T > GlslWriter::declSampler( castor::String const & name
		, uint32_t binding
		, uint32_t dimension )
	{
		using Type = typename TypeOf< T >::Type;
		registerSampler( name, TypeTraits< Type >::TypeEnum, binding, dimension );
		*this << "layout( binding = " << binding << " ) uniform " << TypeTraits< Type >::Name << " " << name << "[" << dimension << "];" << endl;
		return Array< T >( this, binding, name, dimension );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declSampler( castor::String const & name
		, uint32_t binding
		, uint32_t dimension
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerSampler( name, TypeTraits< Type >::TypeEnum, binding, dimension );

		if ( enabled )
		{
			*this << "layout( binding = " << binding << " ) uniform " << TypeTraits< Type >::Name << " " << name << "[" << dimension << "];" << endl;
		}

		return Optional< Array< T > >( this, binding, name, dimension, enabled );
	}

	template< typename T >
	inline T GlslWriter::declAttribute( castor::String const & name )
	{
		using Type = typename TypeOf< T >::Type;
		registerInput( name, TypeTraits< Type >::TypeEnum );
		*this << Attribute() << TypeTraits< Type >::Name << " " << name << cuT( ";" ) << endl;

		if ( std::is_same< T, Mat4 >::value )
		{
			m_attributeIndex += 3;
		}

		return T( this, name );
	}

	template< typename T >
	inline T GlslWriter::declOutput( castor::String const & name )
	{
		using Type = typename TypeOf< T >::Type;
		registerOutput( name, TypeTraits< Type >::TypeEnum );
		*this << Outputgetter< T >() << TypeTraits< Type >::Name << " " << name << cuT( ";" ) << endl;
		return T( this, name );
	}

	template< typename T >
	inline T GlslWriter::declInput( castor::String const & name )
	{
		using Type = typename TypeOf< T >::Type;
		registerInput( name, TypeTraits< Type >::TypeEnum );
		*this << Inputgetter< T >() << TypeTraits< Type >::Name << " " << name << cuT( ";" ) << endl;
		return T( this, name );
	}

	template< typename T >
	inline T GlslWriter::declLocale( castor::String const & name )
	{
		using Type = typename TypeOf< T >::Type;
		registerName( name, TypeTraits< Type >::TypeEnum );
		*this << TypeTraits< Type >::Name << " " << name << cuT( ";" ) << endl;
		return T( this, name );
	}

	template< typename T >
	inline T GlslWriter::declLocale( castor::String const & name
		, T const & rhs )
	{
		using Type = typename TypeOf< T >::Type;
		registerName( name, TypeTraits< Type >::TypeEnum );
		m_stream << TypeTraits< Type >::Name << " " << name << cuT( " = " ) << castor::String( rhs ) << cuT( ";" ) << std::endl;
		T result( this, name );
		return result;
	}

	template< typename T >
	inline T GlslWriter::declBuiltin( castor::String const & name )
	{
		registerName( name, TypeTraits< T >::TypeEnum );
		return T( this, name );
	}

	template< typename T >
	inline T GlslWriter::getBuiltin( castor::String const & name )
	{
		checkNameExists( name, TypeTraits< T >::TypeEnum );
		return T( this, name );
	}

	template< typename T >
	inline T GlslWriter::declUniform( castor::String const & name )
	{
		using Type = typename TypeOf< T >::Type;
		registerUniform( name, TypeTraits< Type >::TypeEnum, 1u );
		*this << Uniform() << TypeTraits< Type >::Name << " " << name << cuT( ";" ) << endl;
		return T( this, name );
	}

	template< typename T >
	inline T GlslWriter::declUniform( castor::String const & name
		, T const & rhs )
	{
		using Type = typename TypeOf< T >::Type;
		registerUniform( name, TypeTraits< Type >::TypeEnum, 1u );
		*this << Uniform() << TypeTraits< Type >::Name << " " << name << cuT( " = " );
		m_stream << castor::String( rhs ) << cuT( ";" ) << std::endl;
		return T( this, name );
	}

	template< typename T >
	inline T GlslWriter::declFragData( castor::String const & name
		, uint32_t index )
	{
		using Type = typename TypeOf< T >::Type;
		registerOutput( name, TypeTraits< Type >::TypeEnum );
		castor::String realName;

		if ( m_keywords->hasNamedFragData() )
		{
			*this << Layout { int( index ) } << Outputgetter< T >() << TypeTraits< Type >::Name << " " << name << cuT( ";" ) << endl;
			realName = name;
		}
		else
		{
			realName = m_keywords->getFragData( index );
		}

		return T( this, realName );
	}

	template< typename T >
	inline Array< T > GlslWriter::declAttribute( castor::String const & name
		, uint32_t dimension )
	{
		using Type = typename TypeOf< T >::Type;
		registerInput( name, TypeTraits< Type >::TypeEnum );
		*this << Attribute() << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "];" ) << endl;

		if ( std::is_same< T, Mat4 >::value )
		{
			m_attributeIndex += 3;
		}

		return Array< T >( this, name, dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::declOutput( castor::String const & name
		, uint32_t dimension )
	{
		using Type = typename TypeOf< T >::Type;
		registerOutput( name, TypeTraits< Type >::TypeEnum );
		*this << Outputgetter< T >() << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "];" ) << endl;
		return Array< T >( this, name, dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::declOutputArray( castor::String const & name )
	{
		using Type = typename TypeOf< T >::Type;
		registerOutput( name, TypeTraits< Type >::TypeEnum );
		*this << Outputgetter< T >() << TypeTraits< Type >::Name << " " << name << cuT( "[];" ) << endl;
		return Array< T >( this, name, 32u );
	}

	template< typename T >
	inline Array< T > GlslWriter::declInput( castor::String const & name
		, uint32_t dimension )
	{
		using Type = typename TypeOf< T >::Type;
		registerInput( name, TypeTraits< Type >::TypeEnum );
		*this << Inputgetter< T >() << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "];" ) << endl;
		return Array< T >( this, name, dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::declInputArray( castor::String const & name )
	{
		using Type = typename TypeOf< T >::Type;
		registerInput( name, TypeTraits< Type >::TypeEnum );
		*this << Inputgetter< T >() << TypeTraits< Type >::Name << " " << name << cuT( "[];" ) << endl;
		return Array< T >( this, name, 32u );
	}

	template< typename T >
	inline Array< T > GlslWriter::declLocale( castor::String const & name
		, uint32_t dimension )
	{
		using Type = typename TypeOf< T >::Type;
		registerName( name, TypeTraits< Type >::TypeEnum );
		*this << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "]" ) << cuT( ";" ) << endl;
		return Array< T >( this, name, dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::declLocale( castor::String const & name
		, uint32_t dimension
		, T const & rhs )
	{
		using Type = typename TypeOf< T >::Type;
		registerName( name, TypeTraits< Type >::TypeEnum );
		m_stream << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "]" ) << cuT( " = " ) << castor::String( rhs ) << cuT( ";" ) << std::endl;
		return Array< T >( this, name );
	}

	template< typename T >
	inline Array< T > GlslWriter::declBuiltin( castor::String const & name
		, uint32_t dimension )
	{
		registerName( name, TypeTraits< Type >::TypeEnum );
		return Array< T >( this, name, dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::declBuiltinArray( castor::String const & name )
	{
		registerName( name, TypeTraits< T >::TypeEnum );
		return Array< T >( this, name, 0xFFFFFFFF );
	}

	template< typename T >
	inline Array< T > GlslWriter::getBuiltin( castor::String const & name
		, uint32_t dimension )
	{
		checkNameExists( name, TypeTraits< T >::TypeEnum );
		return Array< T >( this, name, dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::getBuiltinArray( castor::String const & name )
	{
		checkNameExists( name, TypeTraits< T >::TypeEnum );
		return Array< T >( this, name, 0xFFFFFFFF );
	}

	template< typename T >
	inline Array< T > GlslWriter::declUniform( castor::String const & name
		, uint32_t dimension )
	{
		using Type = typename TypeOf< T >::Type;
		registerUniform( name, TypeTraits< Type >::TypeEnum, dimension );
		*this << Uniform() << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "];" ) << endl;
		return Array< T >( this, name, dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::declUniformArray( castor::String const & name )
	{
		using Type = typename TypeOf< T >::Type;
		registerUniform( name, TypeTraits< Type >::TypeEnum, 0xFFFFFFFFu );
		*this << Uniform() << TypeTraits< Type >::Name << " " << name << cuT( "[];" ) << endl;
		return Array< T >( this, name, 0xFFFFFFFF );
	}

	template< typename T >
	inline Array< T > GlslWriter::declUniform( castor::String const & name
		, uint32_t dimension
		, std::vector< T > const & rhs )
	{
		using Type = typename TypeOf< T >::Type;

		if ( rhs.size() != dimension )
		{
			CASTOR_EXCEPTION( "Given parameters count doesn't match the array dimensions" );
		}
		
		registerUniform( name, TypeTraits< Type >::TypeEnum, dimension );
		*this << Uniform() << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "] = " ) << TypeTraits< Type >::Name << " " << cuT( "[]( " );
		castor::String sep;

		for ( auto const & value : rhs )
		{
			*this << sep << castor::String( value ) << endl;
			sep = cuT( "\t, " );
		}

		*this << cuT( ");" ) << endl;
		return Array< T >( this, name, dimension );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declAttribute( castor::String const & name
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerInput( name, TypeTraits< Type >::TypeEnum );

		if ( enabled )
		{
			*this << Attribute() << TypeTraits< Type >::Name << " " << name << cuT( ";" ) << endl;

			if ( std::is_same< T, Mat4 >::value )
			{
				m_attributeIndex += 3;
			}
		}

		return Optional< T >( this, name, enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declOutput( castor::String const & name
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerOutput( name, TypeTraits< Type >::TypeEnum );

		if ( enabled )
		{
			*this << Outputgetter< T >() << TypeTraits< Type >::Name << " " << name << cuT( ";" ) << endl;
		}

		return Optional< T >( this, name, enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declInput( castor::String const & name
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerInput( name, TypeTraits< Type >::TypeEnum );

		if ( enabled )
		{
			*this << Inputgetter< T >() << TypeTraits< Type >::Name << " " << name << cuT( ";" ) << endl;
		}

		return Optional< T >( this, name, enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declLocale( castor::String const & name
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerName( name, TypeTraits< Type >::TypeEnum );

		if ( enabled )
		{
			*this << TypeTraits< Type >::Name << " " << name << cuT( ";" ) << endl;
		}

		return Optional< T >( this, name, enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declLocale( castor::String const & name
		, Optional< T > const & rhs )
	{
		using Type = typename TypeOf< T >::Type;
		registerName( name, TypeTraits< Type >::TypeEnum );

		if ( rhs.isEnabled() )
		{
			m_stream << TypeTraits< Type >::Name << " " << name << cuT( " = " ) << castor::String( rhs ) << cuT( ";" ) << std::endl;
		}

		return Optional< T >( this, name, rhs.isEnabled() );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declLocale( castor::String const & name
		, bool enabled
		, T const & rhs )
	{
		using Type = typename TypeOf< T >::Type;
		registerName( name, TypeTraits< Type >::TypeEnum );

		if ( enabled )
		{
			m_stream << TypeTraits< Type >::Name << " " << name << cuT( " = " ) << castor::String( rhs ) << cuT( ";" ) << std::endl;
		}

		return Optional< T >( this, name, enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declBuiltin( castor::String const & name
		, bool enabled )
	{
		registerName( name, TypeTraits< T >::TypeEnum );
		return Optional< T >( this, name, enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::getBuiltin( castor::String const & name
		, bool enabled )
	{
		checkNameExists( name, TypeTraits< T >::TypeEnum );
		return Optional< T >( this, name, enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declUniform( castor::String const & name
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerUniform( name, TypeTraits< Type >::TypeEnum, 1u );

		if ( enabled )
		{
			*this << Uniform() << TypeTraits< Type >::Name << " " << name << cuT( ";" ) << endl;
		}

		return Optional< T >( this, name, enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declUniform( castor::String const & name
		, bool enabled
		, T const & rhs )
	{
		using Type = typename TypeOf< T >::Type;
		registerUniform( name, TypeTraits< Type >::TypeEnum, 1u );

		if ( enabled )
		{
			*this << Uniform() << TypeTraits< Type >::Name << " " << name << cuT( " = " );
			m_stream << castor::String( rhs ) << cuT( ";" ) << std::endl;
		}

		return Optional< T >( this, name, enabled );
	}
	
	template< typename T >
	inline Optional< Array< T > > GlslWriter::declAttribute( castor::String const & name
		, uint32_t dimension
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerAttribute( name, TypeTraits< Type >::TypeEnum );

		if ( enabled )
		{
			*this << Attribute() << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "]" ) << cuT( ";" ) << endl;

			if ( std::is_same< T, Mat4 >::value )
			{
				m_attributeIndex += 3;
			}
		}

		return Optional< Array< T > >( this, name, dimension, enabled );
	}
	
	template< typename T >
	inline Optional< Array< T > > GlslWriter::declOutput( castor::String const & name
		, uint32_t dimension
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerOutput( name, TypeTraits< Type >::TypeEnum );

		if ( enabled )
		{
			*this << Outputgetter< T >() << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "]" ) << cuT( ";" ) << endl;
		}

		return Optional< Array< T > >( this, name, dimension, enabled );
	}
	
	template< typename T >
	inline Optional< Array< T > > GlslWriter::declOutputArray( castor::String const & name
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerOutput( name, TypeTraits< Type >::TypeEnum );

		if ( enabled )
		{
			*this << Outputgetter< T >() << TypeTraits< Type >::Name << " " << name << cuT( "[]" ) << cuT( ";" ) << endl;
		}

		return Optional< Array< T > >( this, name, 0xFFFFFFFF, enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declInput( castor::String const & name
		, uint32_t dimension
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerInput( name, TypeTraits< Type >::TypeEnum );

		if ( enabled )
		{
			*this << Inputgetter< T >() << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "]" ) << cuT( ";" ) << endl;
		}

		return Optional< Array< T > >( this, name, dimension, enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declInputArray( castor::String const & name
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerInput( name, TypeTraits< Type >::TypeEnum );

		if ( enabled )
		{
			*this << Inputgetter< T >() << TypeTraits< Type >::Name << " " << name << cuT( "[]" ) << cuT( ";" ) << endl;
		}

		return Optional< Array< T > >( this, name, 0xFFFFFFFF, enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declLocale( castor::String const & name
		, uint32_t dimension
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerName( name, TypeTraits< Type >::TypeEnum );

		if ( enabled )
		{
			*this << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "]" ) << cuT( ";" ) << endl;
		}

		return Optional< Array< T > >( this, name, dimension, enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declLocale( castor::String const & name
		, uint32_t dimension
		, bool enabled
		, T const & rhs )
	{
		using Type = typename TypeOf< T >::Type;
		registerName( name, TypeTraits< Type >::TypeEnum );

		if ( enabled )
		{
			m_stream << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "]" ) << cuT( " = " ) << castor::String( rhs ) << cuT( ";" ) << std::endl;
		}

		return Optional< Array< T > >( this, name, dimension, enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declBuiltin( castor::String const & name
		, uint32_t dimension
		, bool enabled )
	{
		registerName( name, TypeTraits< T >::TypeEnum );
		return Optional< Array< T > >( this, name, dimension, enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declBuiltinArray( castor::String const & name
		, bool enabled )
	{
		registerName( name, TypeTraits< T >::TypeEnum );
		return Optional< Array< T > >( this, name, 0xFFFFFFFF, enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::getBuiltin( castor::String const & name
		, uint32_t dimension
		, bool enabled )
	{
		checkNameExists( name, TypeTraits< T >::TypeEnum );
		return Optional< Array< T > >( this, name, dimension, enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::getBuiltinArray( castor::String const & name
		, bool enabled )
	{
		checkNameExists( name, TypeTraits< T >::TypeEnum );
		return Optional< Array< T > >( this, name, 0xFFFFFFFF, enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declUniform( castor::String const & name
		, uint32_t dimension
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerUniform( name, TypeTraits< Type >::TypeEnum, dimension );

		if ( enabled )
		{
			*this << Uniform() << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "];" ) << endl;
		}

		return Optional< Array< T > >( this, name, dimension, enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declUniformArray( castor::String const & name
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerUniform( name, TypeTraits< Type >::TypeEnum, 0xFFFFFFFFu );

		if ( enabled )
		{
			*this << Uniform() << TypeTraits< Type >::Name << " " << name << cuT( "[];" ) << endl;
		}

		return Optional< Array< T > >( this, name, 0xFFFFFFFF, enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declUniform( castor::String const & name
		, uint32_t dimension
		, bool enabled
		, std::vector< T > const & rhs )
	{
		using Type = typename TypeOf< T >::Type;
		if ( rhs.size() != dimension )
		{
			CASTOR_EXCEPTION( "Given parameters count doesn't match the array dimensions" );
		}

		registerUniform( name, TypeTraits< Type >::TypeEnum, dimension );

		if ( enabled )
		{
			*this << Uniform() << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "] = " ) << TypeTraits< Type >::Name << " " << cuT( "[]( " );
			castor::String sep;

			for ( auto const & value : rhs )
			{
				*this << sep << castor::String( value ) << endl;
				sep = cuT( "\t, " );
			}

			*this << cuT( ");" ) << endl;
		}

		return Optional< Array< T > >( this, name, dimension, enabled );
	}
}
