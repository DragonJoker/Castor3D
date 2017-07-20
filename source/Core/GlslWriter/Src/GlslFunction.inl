namespace GLSL
{
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
		Return result( p_writer );
		result.m_value << p_name;
		Castor::String separator = cuT( "( " );
		WriteFunctionCallRec( separator, result, p_params... );
		return result;
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
		Return result( p_writer );
		result.m_value << result.m_type << cuT( " " ) << p_name;
		Castor::String separator = cuT( "( " );
		WriteFunctionHeaderRec( separator, result, std::forward< Params >( p_params )... );
		WriteLine( *p_writer, Castor::String( result ) );
	}

	template<>
	inline void WriteFunctionHeader< void >( GlslWriter * p_writer, Castor::String const & p_name )
	{
		Void result( p_writer );
		result.m_value << result.m_type << cuT( " " ) << p_name << cuT( "()" );
		WriteLine( *p_writer, Castor::String( result ) );
	}

	template< typename Return, typename ... Params >
	inline void WriteFunctionHeader( GlslWriter & p_writer, Castor::String const & p_name, Params && ... p_params )
	{
		WriteFunctionHeader< Return >( &p_writer, p_name, p_params... );
	}

	//***********************************************************************************************

	template< typename RetT, typename ... ParamsT >
	Function< RetT, ParamsT... >::Function( GlslWriter * p_writer, Castor::String const & p_name )
		: m_writer{ p_writer }
		, m_name{ p_name }
	{
	}

	template< typename RetT, typename ... ParamsT >
	RetT Function< RetT, ParamsT... >::operator()( ParamsT && ... p_params )const
	{
		REQUIRE( !m_name.empty() );
		return WriteFunctionCall< RetT >( m_writer, m_name, std::forward< ParamsT >( p_params )... );
	}

	//***********************************************************************************************
}
