namespace GLSL
{
	namespace
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

		template< typename Return, typename Param, typename ... Params >
		inline void WriteFunctionCallRec( Castor::String & p_separator, Return & p_return, Param const & p_current, Params const & ... p_params );

		template< typename Return >
		inline void WriteFunctionCallRec( Castor::String & p_separator, Return & p_return )
		{
			p_return.m_value << cuT( "()" );
		}

		template< typename Return, typename Param >
		inline void WriteFunctionCallRec( Castor::String & p_separator, Return & p_return, Param const & p_last )
		{
			p_return.m_value << p_separator << ToString( p_last ) << cuT( " )" );
		}

		template< typename Return, typename Param, typename ... Params >
		inline void WriteFunctionCallRec( Castor::String & p_separator, Return & p_return, Param const & p_current, Params const & ... p_params )
		{
			p_return.m_value << p_separator << ToString( p_current );
			p_separator = cuT( ", " );
			WriteFunctionCallRec( p_separator, p_return, p_params... );
		}

		template< typename Return, typename ... Params >
		inline Return WriteFunctionCall( GlslWriter * p_writer, Castor::String const & p_name, Params const & ... p_params )
		{
			Return l_return( p_writer );
			l_return.m_value << p_name;
			Castor::String l_separator = cuT( "( " );
			WriteFunctionCallRec( l_separator, l_return, p_params... );
			return l_return;
		}

		template< typename Return, typename OptType, typename ... Params >
		inline Optional< Return > WriteOptionalFunctionCall( GlslWriter * p_writer, Castor::String const & p_name, Optional< OptType > const & p_param, Params const & ... p_params )
		{
			return Optional< Return >( WriteFunctionCall< Return >( p_writer, p_name, p_param, p_params... ), p_param.IsEnabled() );
		}

		//***********************************************************************************************

		template< typename Return, typename Param, typename ... Params >
		inline void WriteFunctionHeaderRec( Castor::String & p_separator, Return & p_return, Param && p_current, Params && ... p_params );

		template< typename Return >
		inline void WriteFunctionHeaderRec( Castor::String & p_separator, Return & p_return )
		{
			p_return.m_value << cuT( "()" );
		}

		template< typename Return, typename Param >
		inline void WriteFunctionHeaderRec( Castor::String & p_separator, Return & p_return, Param && p_last )
		{
			p_return.m_value << ParamToString( p_separator, p_last ) << cuT( " )" );
		}

		template< typename Return, typename Param, typename ... Params >
		inline void WriteFunctionHeaderRec( Castor::String & p_separator, Return & p_return, Param && p_current, Params && ... p_params )
		{
			p_return.m_value << ParamToString( p_separator, p_current );
			WriteFunctionHeaderRec( p_separator, p_return, std::forward< Params >( p_params )... );
		}

		template< typename Return, typename ... Params >
		inline void WriteFunctionHeader( GlslWriter * p_writer, Castor::String const & p_name, Params && ... p_params )
		{
			Return l_return( p_writer );
			l_return.m_value << l_return.m_type << cuT( " " ) << p_name;
			Castor::String l_separator = cuT( "( " );
			WriteFunctionHeaderRec( l_separator, l_return, std::forward< Params >( p_params )... );
			*p_writer << Castor::String( l_return ) << Endl();
		}

		template<>
		inline void WriteFunctionHeader< void >( GlslWriter * p_writer, Castor::String const & p_name )
		{
			Void l_return( p_writer );
			l_return.m_value << l_return.m_type << cuT( " " ) << p_name << cuT( "()" );
			*p_writer << Castor::String( l_return ) << Endl();
		}

		template< typename Return, typename ... Params >
		inline void WriteFunctionHeader( GlslWriter & p_writer, Castor::String const & p_name, Params && ... p_params )
		{
			WriteFunctionHeader< Return >( &p_writer, p_name, p_params... );
		}
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
	inline T Ubo::GetUniform( Castor::String const & p_name )
	{
		m_stream << m_writer.m_uniform << T().m_type << p_name << cuT( ";" ) << std::endl;
		m_count++;
		return T( &m_writer, p_name );
	}

	template< typename T >
	inline Array< T > Ubo::GetUniform( Castor::String const & p_name, uint32_t p_dimension )
	{
		m_stream << m_writer.m_uniform << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << std::endl;
		m_count++;
		return Array< T >( &m_writer, p_name, p_dimension );
	}

	template< typename T >
	inline Optional< T > Ubo::GetUniform( Castor::String const & p_name, bool p_enabled )
	{
		if ( p_enabled )
		{
			m_stream << m_writer.m_uniform << T().m_type << p_name << cuT( ";" ) << std::endl;
			m_count++;
		}

		return Optional< T >( &m_writer, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > Ubo::GetUniform( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		if ( p_enabled )
		{
			m_stream << m_writer.m_uniform << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( ";" ) << std::endl;
			m_count++;
		}

		return Optional< Array< T > >( &m_writer, p_name, p_dimension, p_enabled );
	}

	//***********************************************************************************************

	template< typename T >
	inline T Struct::GetMember( Castor::String const & p_name )
	{
		m_writer << T().m_type << p_name << cuT( ";" ) << Endl();
		return T( &m_writer, p_name );
	}

	template< typename T >
	inline Array< T > Struct::GetMember( Castor::String const & p_name, uint32_t p_dimension )
	{
		m_writer << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
		return Array< T >( &m_writer, p_name, p_dimension );
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
	inline void GlslWriter::ImplementFunction( Castor::String const & p_name, FuncType p_function, Params && ... p_params )
	{
		m_stream << std::endl;
		WriteFunctionHeader< RetType >( *this, p_name, p_params... );
		{
			IndentBlock l_block( *this );
			p_function( std::forward < Params && > ( p_params )... );
		}
		m_stream << std::endl;
	}

	template< typename RetType >
	inline void GlslWriter::Return( RetType const & p_return )
	{
		m_stream << cuT( "return " ) << Castor::String( p_return ) << cuT( ";" ) << std::endl;
	}

	template< typename ExprType >
	ExprType GlslWriter::Paren( ExprType const p_expr )
	{
		ExprType l_return( this );
		l_return.m_value << cuT( "( " ) << Castor::String( p_expr ) << cuT( " )" );
		return l_return;
	}

	template< typename ExprType >
	ExprType GlslWriter::Ternary( Type const & p_condition, ExprType const & p_left, ExprType const & p_right )
	{
		ExprType l_return( this );
		l_return.m_value << cuT( "( " ) << ToString( p_condition ) << cuT( " ) ? " ) << ToString( p_left ) << cuT( " : " ) << ToString( p_right );
		return l_return;
	}

	template< typename T >
	inline T GlslWriter::Cast( Type const & p_from )
	{
		T l_return;
		l_return.m_value << Castor::string::trim( l_return.m_type ) << cuT( "( " ) << Castor::String( p_from ) << cuT( " )" );
		return l_return;
	}

	template< typename T >
	inline T GlslWriter::GetAttribute( Castor::String const & p_name )
	{
		*this << Attribute() << T().m_type << p_name << cuT( ";" ) << Endl();
		return T( this, p_name );
	}

	template< typename T >
	inline T GlslWriter::GetOutput( Castor::String const & p_name )
	{
		*this << OutputGetter< T >() << T().m_type << p_name << cuT( ";" ) << Endl();
		return T( this, p_name );
	}

	template< typename T >
	inline T GlslWriter::GetInput( Castor::String const & p_name )
	{
		*this << InputGetter< T >() << T().m_type << p_name << cuT( ";" ) << Endl();
		return T( this, p_name );
	}

	template< typename T >
	inline T GlslWriter::GetLocale( Castor::String const & p_name )
	{
		*this << T().m_type << p_name << cuT( ";" ) << Endl();
		return T( this, p_name );
	}

	template< typename T >
	inline T GlslWriter::GetLocale( Castor::String const & p_name, Expr const & p_rhs )
	{
		m_stream << T().m_type << p_name << cuT( " = " ) << p_rhs.m_value.rdbuf() << cuT( ";" ) << std::endl;
		T l_return( this, p_name );
		return l_return;
	}

	template< typename T >
	inline T GlslWriter::GetLocale( Castor::String const & p_name, Type const & p_rhs )
	{
		m_stream << T().m_type << p_name << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << std::endl;
		T l_return( this, p_name );
		return l_return;
	}

	template< typename T >
	inline T GlslWriter::GetBuiltin( Castor::String const & p_name )
	{
		return T( this, p_name );
	}

	template< typename T >
	inline T GlslWriter::GetUniform( Castor::String const & p_name )
	{
		*this << Uniform() << T().m_type << p_name << cuT( ";" ) << Endl();
		return T( this, p_name );
	}

	template< typename T >
	inline T GlslWriter::GetFragData( Castor::String const & p_name, uint32_t p_index )
	{
		Castor::String l_name;

		if ( m_keywords->HasNamedFragData() )
		{
			*this << Layout { int( p_index ) } << OutputGetter< T >() << T().m_type << p_name << cuT( ";" ) << Endl();
			l_name = p_name;
		}
		else
		{
			l_name = m_keywords->GetFragData( p_index );
		}

		return T( this, l_name );
	}

	template< typename T >
	inline Array< T > GlslWriter::GetAttribute( Castor::String const & p_name, uint32_t p_dimension )
	{
		*this << Attribute() << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
		return Array< T >( this, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::GetOutput( Castor::String const & p_name, uint32_t p_dimension )
	{
		*this << OutputGetter< T >() << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
		return Array< T >( this, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::GetInput( Castor::String const & p_name, uint32_t p_dimension )
	{
		*this << InputGetter< T >() << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
		return Array< T >( this, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::GetLocale( Castor::String const & p_name, uint32_t p_dimension )
	{
		*this << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( ";" ) << Endl();
		return Array< T >( this, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::GetLocale( Castor::String const & p_name, uint32_t p_dimension, Expr const & p_rhs )
	{
		m_stream << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( " = " ) << p_rhs.m_value.rdbuf() << cuT( ";" ) << std::endl;
		return Array< T >( this, p_name );
	}

	template< typename T >
	inline Array< T > GlslWriter::GetLocale( Castor::String const & p_name, uint32_t p_dimension, Type const & p_rhs )
	{
		m_stream << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << std::endl;
		return Array< T >( this, p_name );
	}

	template< typename T >
	inline Array< T > GlslWriter::GetBuiltin( Castor::String const & p_name, uint32_t p_dimension )
	{
		return Array< T >( this, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > GlslWriter::GetUniform( Castor::String const & p_name, uint32_t p_dimension )
	{
		*this << Uniform() << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
		return Array< T >( this, p_name, p_dimension );
	}

	template< typename T >
	inline Optional< T > GlslWriter::GetAttribute( Castor::String const & p_name, bool p_enabled )
	{
		if ( p_enabled )
		{
			*this << Attribute() << T().m_type << p_name << cuT( ";" ) << Endl();
		}

		return Optional< T >( this, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::GetOutput( Castor::String const & p_name, bool p_enabled )
	{
		if ( p_enabled )
		{
			*this << OutputGetter< T >() << T().m_type << p_name << cuT( ";" ) << Endl();
		}

		return Optional< T >( this, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::GetInput( Castor::String const & p_name, bool p_enabled )
	{
		if ( p_enabled )
		{
			*this << InputGetter< T >() << T().m_type << p_name << cuT( ";" ) << Endl();
		}

		return Optional< T >( this, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::GetLocale( Castor::String const & p_name, bool p_enabled )
	{
		if ( p_enabled )
		{
			*this << T().m_type << p_name << cuT( ";" ) << Endl();
		}

		return Optional< T >( this, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::GetLocale( Castor::String const & p_name, bool p_enabled, Expr const & p_rhs )
	{
		if ( p_enabled )
		{
			m_stream << T().m_type << p_name << cuT( " = " ) << p_rhs.m_value.rdbuf() << cuT( ";" ) << std::endl;
		}

		return Optional< T >( this, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::GetLocale( Castor::String const & p_name, bool p_enabled, Type const & p_rhs )
	{
		if ( p_enabled )
		{
			m_stream << T().m_type << p_name << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << std::endl;
		}

		return Optional< T >( this, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::GetBuiltin( Castor::String const & p_name, bool p_enabled )
	{
		return Optional< T >( this, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< T > GlslWriter::GetUniform( Castor::String const & p_name, bool p_enabled )
	{
		if ( p_enabled )
		{
			*this << Uniform() << T().m_type << p_name << cuT( ";" ) << Endl();
		}

		return Optional< T >( this, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::GetAttribute( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		if ( p_enabled )
		{
			*this << Attribute() << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( ";" ) << Endl();
		}

		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::GetOutput( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		if ( p_enabled )
		{
			*this << OutputGetter< T >() << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( ";" ) << Endl();
		}

		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::GetInput( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		if ( p_enabled )
		{
			*this << InputGetter< T >() << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( ";" ) << Endl();
		}

		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::GetLocale( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		if ( p_enabled )
		{
			*this << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( ";" ) << Endl();
		}

		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::GetLocale( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled, Expr const & p_rhs )
	{
		if ( p_enabled )
		{
			m_stream << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( " = " ) << p_rhs.m_value.rdbuf() << cuT( ";" ) << std::endl;
		}

		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::GetLocale( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled, Type const & p_rhs )
	{
		if ( p_enabled )
		{
			m_stream << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << std::endl;
		}

		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::GetBuiltin( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > GlslWriter::GetUniform( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		if ( p_enabled )
		{
			*this << Uniform() << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
		}

		return Optional< Array< T > >( this, p_name, p_dimension, p_enabled );
	}
}
