namespace glsl
{
	//***********************************************************************************************

	template< typename T >
	struct OutputGetter
	{
	};

	template< typename T >
	inline GlslWriter & operator<<( GlslWriter & stream, OutputGetter< T > const & in )
	{
		stream << Out();
		return stream;
	}

	template<>
	inline GlslWriter & operator<<( GlslWriter & stream, OutputGetter< Int > const & in )
	{
		stream << "flat " << Out();
		return stream;
	}

	template<>
	inline GlslWriter & operator<<( GlslWriter & stream, OutputGetter< UInt > const & in )
	{
		stream << "flat " << Out();
		return stream;
	}

	//***********************************************************************************************

	template< typename T >
	struct InputGetter
	{
	};

	template< typename T >
	inline GlslWriter & operator<<( GlslWriter & stream, InputGetter< T > const & in )
	{
		stream << In();
		return stream;
	}

	template<>
	inline GlslWriter & operator<<( GlslWriter & stream, InputGetter< Int > const & in )
	{
		stream << "flat " << In();
		return stream;
	}

	template<>
	inline GlslWriter & operator<<( GlslWriter & stream, InputGetter< UInt > const & in )
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
			function( std::forward< Params && > ( params )... );
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
#pragma region Constant declaration
	/**
	*name
	*	Constant declaration.
	*/
	/**@{*/
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
	/**@}*/
#pragma endregion
#pragma region Specialisation constant declaration
	/**
	*name
	*	Specialisation constant declaration.
	*/
	/**@{*/
	template< typename T >
	inline T GlslWriter::declSpecConstant( castor::String const & name
		, uint32_t location
		, T const & rhs )
	{
		using Type = typename TypeOf< T >::Type;
		registerConstant( name, TypeTraits< Type >::TypeEnum );

		if ( m_config.m_hasSpecialisationConstants )
		{
			*this << "layout( constant_id = " << location << " ) const " << TypeTraits< Type >::Name << " " << name << " = ";
		}
		else
		{
			*this << "layout( location = " << location << " ) uniform " << TypeTraits< Type >::Name << " " << name << " = ";
		}

		m_stream << castor::String( rhs ) << ";\n";
		return T( this, name );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declSpecConstant( castor::String const & name
		, uint32_t location
		, T const & rhs
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerConstant( name, TypeTraits< Type >::TypeEnum );

		if ( enabled )
		{
			if ( m_config.m_hasSpecialisationConstants )
			{
				*this << "layout( constant_id = " << location << " ) const " << TypeTraits< Type >::Name << " " << name << " = ";
			}
			else
			{
				*this << "layout( location = " << location << " ) uniform " << TypeTraits< Type >::Name << " " << name << " = ";
			}

			m_stream << castor::String( rhs ) << ";\n";
		}

		return Optional< T >( this, name, enabled );
	}
	/**@}*/
#pragma endregion
#pragma region Sampler declaration
	/**
	*name
	*	Sampler declaration.
	*/
	/**@{*/
	template< typename T >
	inline T GlslWriter::declSampler( castor::String const & name
		, uint32_t binding )
	{
		using Type = typename TypeOf< T >::Type;
		registerSampler( name, TypeTraits< Type >::TypeEnum, binding, 0u, 1u );
		*this << m_keywords->getTextureLayout( binding, 0u ) << cuT( "uniform " ) << TypeTraits< Type >::Name << " " << name << ";" << endl;
		return T( this, binding, name );
	}

	template< typename T >
	inline T GlslWriter::declSampler( castor::String const & name
		, uint32_t binding
		, uint32_t set )
	{
		using Type = typename TypeOf< T >::Type;
		registerSampler( name, TypeTraits< Type >::TypeEnum, binding, set, 1u );
		*this << m_keywords->getTextureLayout( binding, set ) << cuT( "uniform " ) << TypeTraits< Type >::Name << " " << name << ";" << endl;
		return T( this, binding, name );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declSampler( castor::String const & name
		, uint32_t binding
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerSampler( name, TypeTraits< Type >::TypeEnum, binding, 0u, 1u, enabled );

		if ( enabled )
		{
			*this << m_keywords->getTextureLayout( binding, 0u ) << cuT( "uniform " ) << TypeTraits< Type >::Name << " " << name << ";" << endl;
		}

		return Optional< T >( this, binding, name, enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declSampler( castor::String const & name
		, uint32_t binding
		, uint32_t set
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerSampler( name, TypeTraits< Type >::TypeEnum, binding, set, 1u, enabled );

		if ( enabled )
		{
			*this << m_keywords->getTextureLayout( binding, set ) << cuT( "uniform " ) << TypeTraits< Type >::Name << " " << name << ";" << endl;
		}

		return Optional< T >( this, binding, name, enabled );
	}

	template< typename T >
	inline Array< T > GlslWriter::declSamplerArray( castor::String const & name
		, uint32_t binding
		, uint32_t dimension )
	{
		using Type = typename TypeOf< T >::Type;
		registerSampler( name, TypeTraits< Type >::TypeEnum, binding, 0u, dimension );
		*this << m_keywords->getTextureLayout( binding, 0u ) << cuT( "uniform " ) << TypeTraits< Type >::Name << " " << name << ";" << endl;
		return Array< T >( this, binding, name, dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::declSamplerArray( castor::String const & name
		, uint32_t binding
		, uint32_t set
		, uint32_t dimension )
	{
		using Type = typename TypeOf< T >::Type;
		registerSampler( name, TypeTraits< Type >::TypeEnum, binding, set, dimension );
		*this << m_keywords->getTextureLayout( binding, set ) << cuT( "uniform " ) << TypeTraits< Type >::Name << " " << name << "[" << dimension << "];" << endl;
		return Array< T >( this, binding, name, dimension );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declSamplerArray( castor::String const & name
		, uint32_t binding
		, uint32_t dimension
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerSampler( name, TypeTraits< Type >::TypeEnum, binding, 0u, dimension, enabled );

		if ( enabled )
		{
			*this << m_keywords->getTextureLayout( binding, 0u ) << cuT( "uniform " ) << TypeTraits< Type >::Name << " " << name << "[" << dimension << "];" << endl;
		}

		return Optional< Array< T > >( this, binding, name, dimension, enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declSamplerArray( castor::String const & name
		, uint32_t binding
		, uint32_t set
		, uint32_t dimension
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerSampler( name, TypeTraits< Type >::TypeEnum, binding, set, dimension, enabled );

		if ( enabled )
		{
			*this << m_keywords->getTextureLayout( binding, set ) << cuT( "uniform " ) << TypeTraits< Type >::Name << " " << name << "[" << dimension << "];" << endl;
		}

		return Optional< Array< T > >( this, binding, name, dimension, enabled );
	}
	/**@}*/
#pragma endregion
#pragma region Uniform Declaration
	/**
	*name
	*	Uniform declaration.
	*/
	/**@{*/
	template< typename T >
	inline T GlslWriter::declUniform( castor::String const & name
		, uint32_t location )
	{
		using Type = typename TypeOf< T >::Type;
		registerUniform( name, location, TypeTraits< Type >::TypeEnum, 1u );
		*this << m_keywords->getUniformLayout( location ) << Uniform{} << TypeTraits< Type >::Name << " " << name << cuT( ";" ) << endl;
		return T( this, name );
	}

	template< typename T >
	inline T GlslWriter::declUniform( castor::String const & name
		, uint32_t location
		, T const & rhs )
	{
		using Type = typename TypeOf< T >::Type;
		registerUniform( name, location, TypeTraits< Type >::TypeEnum, 1u );
		*this << m_keywords->getUniformLayout( location ) << Uniform{} << TypeTraits< Type >::Name << " " << name << cuT( " = " );
		m_stream << castor::String( rhs ) << cuT( ";" ) << std::endl;
		return T( this, name );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declUniform( castor::String const & name
		, uint32_t location
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerUniform( name, location, TypeTraits< Type >::TypeEnum, 1u );

		if ( enabled )
		{
			*this << m_keywords->getUniformLayout( location ) << Uniform{} << TypeTraits< Type >::Name << " " << name << cuT( ";" ) << endl;
		}

		return Optional< T >( this, name, enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declUniform( castor::String const & name
		, uint32_t location
		, bool enabled
		, T const & rhs )
	{
		using Type = typename TypeOf< T >::Type;
		registerUniform( name, location, TypeTraits< Type >::TypeEnum, 1u );

		if ( enabled )
		{
			*this << m_keywords->getUniformLayout( location ) << Uniform{} << TypeTraits< Type >::Name << " " << name << cuT( " = " );
			m_stream << castor::String( rhs ) << cuT( ";" ) << std::endl;
		}

		return Optional< T >( this, name, enabled );
	}

	template< typename T >
	inline Array< T > GlslWriter::declUniformArray( castor::String const & name
		, uint32_t location
		, uint32_t dimension )
	{
		using Type = typename TypeOf< T >::Type;
		registerUniform( name, location, TypeTraits< Type >::TypeEnum, dimension );
		*this << m_keywords->getUniformLayout( location ) << Uniform{} << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "];" ) << endl;
		return Array< T >( this, name, dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::declUniformArray( castor::String const & name
		, uint32_t location )
	{
		using Type = typename TypeOf< T >::Type;
		registerUniform( name, location, TypeTraits< Type >::TypeEnum, 0xFFFFFFFFu );
		*this << m_keywords->getUniformLayout( location ) << Uniform{} << TypeTraits< Type >::Name << " " << name << cuT( "[];" ) << endl;
		return Array< T >( this, name, 0xFFFFFFFF );
	}

	template< typename T >
	inline Array< T > GlslWriter::declUniformArray( castor::String const & name
		, uint32_t location
		, uint32_t dimension
		, std::vector< T > const & rhs )
	{
		using Type = typename TypeOf< T >::Type;

		if ( rhs.size() != dimension )
		{
			CASTOR_EXCEPTION( "Given parameters count doesn't match the array dimensions" );
		}

		registerUniform( name, location, TypeTraits< Type >::TypeEnum, dimension );
		*this << m_keywords->getUniformLayout( location ) << Uniform{} << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "] = " ) << TypeTraits< Type >::Name << " " << cuT( "[]( " );
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
	inline Optional< Array< T > > GlslWriter::declUniformArray( castor::String const & name
		, uint32_t location
		, uint32_t dimension
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerUniform( name, location, TypeTraits< Type >::TypeEnum, dimension );

		if ( enabled )
		{
			*this << m_keywords->getUniformLayout( location ) << Uniform{} << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "];" ) << endl;
		}

		return Optional< Array< T > >( this, name, dimension, enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declUniformArray( castor::String const & name
		, uint32_t location
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerUniform( name, location, TypeTraits< Type >::TypeEnum, 0xFFFFFFFFu );

		if ( enabled )
		{
			*this << m_keywords->getUniformLayout( location ) << Uniform{} << TypeTraits< Type >::Name << " " << name << cuT( "[];" ) << endl;
		}

		return Optional< Array< T > >( this, name, 0xFFFFFFFF, enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declUniformArray( castor::String const & name
		, uint32_t location
		, uint32_t dimension
		, bool enabled
		, std::vector< T > const & rhs )
	{
		using Type = typename TypeOf< T >::Type;
		if ( rhs.size() != dimension )
		{
			CASTOR_EXCEPTION( "Given parameters count doesn't match the array dimensions" );
		}

		registerUniform( name, location, TypeTraits< Type >::TypeEnum, dimension );

		if ( enabled )
		{
			*this << m_keywords->getUniformLayout( location ) << Uniform{} << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "] = " ) << TypeTraits< Type >::Name << " " << cuT( "[]( " );
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
	/**@}*/
#pragma endregion
#pragma region Input declaration
	/**
	*name
	*	Input declaration.
	*/
	/**@{*/
	template< typename T >
	inline T GlslWriter::declAttribute( castor::String const & name
		, uint32_t location )
	{
		using Type = typename TypeOf< T >::Type;
		registerInput( name, location, TypeTraits< Type >::TypeEnum );
		*this << m_keywords->getInputLayout( location ) << Attribute{} << TypeTraits< Type >::Name << " " << name << cuT( ";" ) << endl;

		if ( std::is_same< T, Mat4 >::value )
		{
			m_attributeIndex += 3;
		}

		return T( this, name );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declAttribute( castor::String const & name
		, uint32_t location
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerInput( name, location, TypeTraits< Type >::TypeEnum );

		if ( enabled )
		{
			*this << m_keywords->getInputLayout( location ) << Attribute{} << TypeTraits< Type >::Name << " " << name << cuT( ";" ) << endl;

			if ( std::is_same< T, Mat4 >::value )
			{
				m_attributeIndex += 3;
			}
		}

		return Optional< T >( this, name, enabled );
	}

	template< typename T >
	inline Array< T > GlslWriter::declAttributeArray( castor::String const & name
		, uint32_t location
		, uint32_t dimension )
	{
		using Type = typename TypeOf< T >::Type;
		registerInput( name, location, TypeTraits< Type >::TypeEnum );
		*this << m_keywords->getInputLayout( location ) << Attribute{} << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "];" ) << endl;

		if ( std::is_same< T, Mat4 >::value )
		{
			m_attributeIndex += 3;
		}

		return Array< T >( this, name, dimension );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declAttributeArray( castor::String const & name
		, uint32_t location
		, uint32_t dimension
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerAttribute( name, location, TypeTraits< Type >::TypeEnum );

		if ( enabled )
		{
			*this << m_keywords->getInputLayout( location ) << Attribute{} << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "]" ) << cuT( ";" ) << endl;

			if ( std::is_same< T, Mat4 >::value )
			{
				m_attributeIndex += 3;
			}
		}

		return Optional< Array< T > >( this, name, dimension, enabled );
	}

	template< typename T >
	inline T GlslWriter::declInput( castor::String const & name
		, uint32_t location )
	{
		using Type = typename TypeOf< T >::Type;
		registerInput( name, location, TypeTraits< Type >::TypeEnum );
		*this << m_keywords->getInputLayout( location ) << InputGetter< T >{} << TypeTraits< Type >::Name << " " << name << cuT( ";" ) << endl;
		return T( this, name );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declInput( castor::String const & name
		, uint32_t location
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerInput( name, location, TypeTraits< Type >::TypeEnum );

		if ( enabled )
		{
			*this << m_keywords->getInputLayout( location ) << InputGetter< T >{} << TypeTraits< Type >::Name << " " << name << cuT( ";" ) << endl;
		}

		return Optional< T >( this, name, enabled );
	}

	template< typename T >
	inline Array< T > GlslWriter::declInputArray( castor::String const & name
		, uint32_t location
		, uint32_t dimension )
	{
		using Type = typename TypeOf< T >::Type;
		registerInput( name, location, TypeTraits< Type >::TypeEnum );
		*this << m_keywords->getInputLayout( location ) << InputGetter< T >{} << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "];" ) << endl;
		return Array< T >( this, name, dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::declInputArray( castor::String const & name
		, uint32_t location )
	{
		using Type = typename TypeOf< T >::Type;
		registerInput( name, location, TypeTraits< Type >::TypeEnum );
		*this << m_keywords->getInputLayout( location ) << InputGetter< T >{} << TypeTraits< Type >::Name << " " << name << cuT( "[];" ) << endl;
		return Array< T >( this, name, 32u );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declInputArray( castor::String const & name
		, uint32_t location
		, uint32_t dimension
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerInput( name, location, TypeTraits< Type >::TypeEnum );

		if ( enabled )
		{
			*this << m_keywords->getInputLayout( location ) << InputGetter< T >{} << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "]" ) << cuT( ";" ) << endl;
		}

		return Optional< Array< T > >( this, name, dimension, enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declInputArray( castor::String const & name
		, uint32_t location
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerInput( name, location, TypeTraits< Type >::TypeEnum );

		if ( enabled )
		{
			*this << m_keywords->getInputLayout( location ) << InputGetter< T >{} << TypeTraits< Type >::Name << " " << name << cuT( "[]" ) << cuT( ";" ) << endl;
		}

		return Optional< Array< T > >( this, name, 0xFFFFFFFF, enabled );
	}
	/**@}*/
#pragma endregion
#pragma region Output declaration
	/**
	*name
	*	Output declaration.
	*/
	/**@{*/
	template< typename T >
	inline T GlslWriter::declFragData( castor::String const & name
		, uint32_t location )
	{
		using Type = typename TypeOf< T >::Type;
		registerOutput( name, location, TypeTraits< Type >::TypeEnum );
		castor::String realName;

		if ( m_keywords->hasNamedFragData() )
		{
			*this << m_keywords->getOutputLayout( location ) << OutputGetter< T >{} << TypeTraits< Type >::Name << " " << name << cuT( ";" ) << endl;
			realName = name;
		}
		else
		{
			realName = m_keywords->getFragData( location );
		}

		return T( this, realName );
	}

	template< typename T >
	inline T GlslWriter::declOutput( castor::String const & name
		, uint32_t location )
	{
		using Type = typename TypeOf< T >::Type;
		registerOutput( name, location, TypeTraits< Type >::TypeEnum );
		*this << m_keywords->getOutputLayout( location ) << OutputGetter< T >{} << TypeTraits< Type >::Name << " " << name << cuT( ";" ) << endl;
		return T( this, name );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declOutput( castor::String const & name
		, uint32_t location
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerOutput( name, location, TypeTraits< Type >::TypeEnum );

		if ( enabled )
		{
			*this << m_keywords->getOutputLayout( location ) << OutputGetter< T >{} << TypeTraits< Type >::Name << " " << name << cuT( ";" ) << endl;
		}

		return Optional< T >( this, name, enabled );
	}

	template< typename T >
	inline Array< T > GlslWriter::declOutputArray( castor::String const & name
		, uint32_t location
		, uint32_t dimension )
	{
		using Type = typename TypeOf< T >::Type;
		registerOutput( name, location, TypeTraits< Type >::TypeEnum );
		*this << m_keywords->getOutputLayout( location ) << OutputGetter< T >{} << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "];" ) << endl;
		return Array< T >( this, name, dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::declOutputArray( castor::String const & name
		, uint32_t location )
	{
		using Type = typename TypeOf< T >::Type;
		registerOutput( name, location, TypeTraits< Type >::TypeEnum );
		*this << m_keywords->getOutputLayout( location ) << OutputGetter< T >{} << TypeTraits< Type >::Name << " " << name << cuT( "[];" ) << endl;
		return Array< T >( this, name, 32u );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declOutputArray( castor::String const & name
		, uint32_t location
		, uint32_t dimension
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerOutput( name, location, TypeTraits< Type >::TypeEnum );

		if ( enabled )
		{
			*this << m_keywords->getOutputLayout( location ) << OutputGetter< T >{} << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "]" ) << cuT( ";" ) << endl;
		}

		return Optional< Array< T > >( this, name, dimension, enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declOutputArray( castor::String const & name
		, uint32_t location
		, bool enabled )
	{
		using Type = typename TypeOf< T >::Type;
		registerOutput( name, location, TypeTraits< Type >::TypeEnum );

		if ( enabled )
		{
			*this << m_keywords->getOutputLayout( location ) << OutputGetter< T >{} << TypeTraits< Type >::Name << " " << name << cuT( "[]" ) << cuT( ";" ) << endl;
		}

		return Optional< Array< T > >( this, name, 0xFFFFFFFF, enabled );
	}
	/**@}*/
#pragma endregion
#pragma region Locale declaration
	/**
	*name
	*	Locale declaration.
	*/
	/**@{*/
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
	inline Array< T > GlslWriter::declLocaleArray( castor::String const & name
		, uint32_t dimension )
	{
		using Type = typename TypeOf< T >::Type;
		registerName( name, TypeTraits< Type >::TypeEnum );
		*this << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "]" ) << cuT( ";" ) << endl;
		return Array< T >( this, name, dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::declLocaleArray( castor::String const & name
		, uint32_t dimension
		, std::vector< T > const & rhs )
	{
		using Type = typename TypeOf< T >::Type;
		registerName( name, TypeTraits< Type >::TypeEnum );
		*this << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "] = " ) << TypeTraits< Type >::Name << " " << cuT( "[]( " );
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
	inline Optional< Array< T > > GlslWriter::declLocaleArray( castor::String const & name
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
	inline Optional< Array< T > > GlslWriter::declLocaleArray( castor::String const & name
		, uint32_t dimension
		, bool enabled
		, std::vector< T > const & rhs )
	{
		using Type = typename TypeOf< T >::Type;
		registerName( name, TypeTraits< Type >::TypeEnum );

		if ( enabled )
		{
			*this << TypeTraits< Type >::Name << " " << name << cuT( "[" ) << dimension << cuT( "] = " ) << TypeTraits< Type >::Name << " " << cuT( "[]( " );
			castor::String sep;

			for ( auto const & value : rhs )
			{
				*this << sep << castor::String( value ) << endl;
				sep = cuT( "\t, " );
			}
		}

		return Optional< Array< T > >( this, name, dimension, enabled );
	}
	/**@}*/
#pragma endregion
#pragma region Built-in declaration
	/**
	*name
	*	Built-in variable declaration.
	*/
	/**@{*/
	template< typename T >
	inline T GlslWriter::declBuiltin( castor::String const & name )
	{
		registerName( name, TypeTraits< T >::TypeEnum );
		return T( this, name );
	}

	template< typename T >
	inline Optional< T > GlslWriter::declBuiltin( castor::String const & name
		, bool enabled )
	{
		registerName( name, TypeTraits< T >::TypeEnum );
		return Optional< T >( this, name, enabled );
	}

	template< typename T >
	inline Array< T > GlslWriter::declBuiltinArray( castor::String const & name
		, uint32_t dimension )
	{
		registerName( name, TypeTraits< T >::TypeEnum );
		return Array< T >( this, name, dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::declBuiltinArray( castor::String const & name )
	{
		registerName( name, TypeTraits< T >::TypeEnum );
		return Array< T >( this, name, 0xFFFFFFFF );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::declBuiltinArray( castor::String const & name
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
	inline T GlslWriter::getBuiltin( castor::String const & name )
	{
		checkNameExists( name, TypeTraits< T >::TypeEnum );
		return T( this, name );
	}

	template< typename T >
	inline Optional< T > GlslWriter::getBuiltin( castor::String const & name
		, bool enabled )
	{
		checkNameExists( name, TypeTraits< T >::TypeEnum );
		return Optional< T >( this, name, enabled );
	}

	template< typename T >
	inline Array< T > GlslWriter::getBuiltinArray( castor::String const & name )
	{
		checkNameExists( name, TypeTraits< T >::TypeEnum );
		return Array< T >( this, name, 0xFFFFFFFF );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::getBuiltinArray( castor::String const & name
		, bool enabled )
	{
		checkNameExists( name, TypeTraits< T >::TypeEnum );
		return Optional< Array< T > >( this, name, 0xFFFFFFFF, enabled );
	}
	/**@}*/
#pragma endregion
}
