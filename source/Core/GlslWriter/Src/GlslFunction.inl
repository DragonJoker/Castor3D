namespace glsl
{
	//***********************************************************************************************

	template< typename Return, typename Param, typename ... Params >
	inline void writeFunctionCallRec( castor::String & p_separator, Return & p_return, Param const & p_current, Params const & ... p_params );

	template< typename Return >
	inline void writeFunctionCallRec( castor::String & p_separator, Return & p_return )
	{
		p_return.m_value << cuT( "()" );
	}

	template< typename Return, typename Param >
	inline void writeFunctionCallRec( castor::String & p_separator, Return & p_return, Param const & p_last )
	{
		p_return.m_value << p_separator << toString( p_last ) << cuT( " )" );
	}

	template< typename Return, typename Param, typename ... Params >
	inline void writeFunctionCallRec( castor::String & p_separator, Return & p_return, Param const & p_current, Params const & ... p_params )
	{
		p_return.m_value << p_separator << toString( p_current );
		p_separator = cuT( ", " );
		writeFunctionCallRec( p_separator, p_return, p_params... );
	}

	template< typename Return, typename ... Params >
	inline Return writeFunctionCall( GlslWriter * p_writer, castor::String const & p_name, Params const & ... p_params )
	{
		Return result( p_writer );
		result.m_value << p_name;
		castor::String separator = cuT( "( " );
		writeFunctionCallRec( separator, result, p_params... );
		return result;
	}

	template< typename Return, typename OptType, typename ... Params >
	inline Optional< Return > writeOptionalFunctionCall( GlslWriter * p_writer, castor::String const & p_name, Optional< OptType > const & p_param, Params const & ... p_params )
	{
		return Optional< Return >( writeFunctionCall< Return >( p_writer, p_name, p_param, p_params... ), p_param.isEnabled() );
	}

	//***********************************************************************************************

	template< typename Return, typename Param, typename ... Params >
	inline void writeFunctionHeaderRec( castor::String & p_separator, Return & p_return, Param && p_current, Params && ... p_params );

	template< typename Return >
	inline void writeFunctionHeaderRec( castor::String & p_separator, Return & p_return )
	{
		p_return.m_value << cuT( "()" );
	}

	template< typename Return, typename Param >
	inline void writeFunctionHeaderRec( castor::String & p_separator, Return & p_return, Param && p_last )
	{
		p_return.m_value << paramToString( p_separator, p_last ) << cuT( " )" );
	}

	template< typename Return, typename Param, typename ... Params >
	inline void writeFunctionHeaderRec( castor::String & p_separator, Return & p_return, Param && p_current, Params && ... p_params )
	{
		p_return.m_value << paramToString( p_separator, p_current );
		writeFunctionHeaderRec( p_separator, p_return, std::forward< Params >( p_params )... );
	}

	template< typename Return, typename ... Params >
	inline void writeFunctionHeader( GlslWriter * p_writer, castor::String const & p_name, Params && ... p_params )
	{
		Return result( p_writer );
		result.m_value << result.m_type << cuT( " " ) << p_name;
		castor::String separator = cuT( "( " );
		writeFunctionHeaderRec( separator, result, std::forward< Params >( p_params )... );
		writeLine( *p_writer, castor::String( result ) );
	}

	template<>
	inline void writeFunctionHeader< void >( GlslWriter * p_writer, castor::String const & p_name )
	{
		Void result( p_writer );
		result.m_value << result.m_type << cuT( " " ) << p_name << cuT( "()" );
		writeLine( *p_writer, castor::String( result ) );
	}

	template< typename Return, typename ... Params >
	inline void writeFunctionHeader( GlslWriter & p_writer, castor::String const & p_name, Params && ... p_params )
	{
		writeFunctionHeader< Return >( &p_writer, p_name, p_params... );
	}

	//***********************************************************************************************

	template< typename RetT, typename ... ParamsT >
	Function< RetT, ParamsT... >::Function( GlslWriter * p_writer, castor::String const & p_name )
		: m_writer{ p_writer }
		, m_name{ p_name }
	{
	}

	template< typename RetT, typename ... ParamsT >
	RetT Function< RetT, ParamsT... >::operator()( ParamsT && ... p_params )const
	{
		REQUIRE( !m_name.empty() );
		return writeFunctionCall< RetT >( m_writer, m_name, std::forward< ParamsT >( p_params )... );
	}

	//***********************************************************************************************
}
