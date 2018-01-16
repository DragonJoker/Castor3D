namespace glsl
{
	namespace details
	{
		template< typename T >
		struct RemoveOptional
		{
			using Type = T;
		};

		template< typename T >
		struct RemoveOptional< Optional< T > >
		{
			using Type = T;
		};

		inline GlslWriter * getWriter( Type const & value )
		{
			return value.m_writer;
		}

		inline GlslWriter * getWriter( int32_t const & value )
		{
			return nullptr;
		}

		inline GlslWriter * getWriter( uint32_t const & value )
		{
			return nullptr;
		}

		inline GlslWriter * getWriter( float const & value )
		{
			return nullptr;
		}

		inline GlslWriter * getWriter( double const & value )
		{
			return nullptr;
		}
	}

	//***********************************************************************************************

	template< typename Value, typename ... Values >
	inline void findWriterRec( GlslWriter *& result, Value const & current, Values const & ... values );

	template< typename Value >
	inline void findWriterRec( GlslWriter *& result, Value const & last )
	{
		if ( !result )
		{
			result = details::getWriter( last );
		}
	}

	template< typename Value, typename ... Values >
	inline void findWriterRec( GlslWriter *& result, Value const & current, Values const & ... values )
	{
		if ( !result )
		{
			result = details::getWriter( current );
			findWriterRec( result, values... );
		}
	}

	template< typename ... ValuesT >
	inline GlslWriter * findWriter( ValuesT const & ... values )
	{
		GlslWriter * result{ nullptr };
		findWriterRec( result, values... );
		return result;
	}

	//***********************************************************************************************

	template< typename Return, typename Param, typename ... Params >
	inline void writeFunctionCallRec( castor::String & separator, Return & result, Param const & current, Params const & ... params );

	template< typename Return >
	inline void writeFunctionCallRec( castor::String & separator, Return & result )
	{
		result.m_value << cuT( "()" );
	}

	template< typename Return, typename Param >
	inline void writeFunctionCallRec( castor::String & separator, Return & result, Param const & p_last )
	{
		result.m_value << separator << toString( p_last ) << cuT( " )" );
	}

	template< typename Return, typename Param, typename ... Params >
	inline void writeFunctionCallRec( castor::String & separator, Return & result, Param const & current, Params const & ... params )
	{
		result.m_value << separator << toString( current );
		separator = cuT( ", " );
		writeFunctionCallRec( separator, result, params... );
	}

	template< typename Return, typename ... Params >
	inline Return writeFunctionCall( GlslWriter * writer, castor::String const & name, Params const & ... params )
	{
		Return result( writer );
		result.m_value << name;
		castor::String separator = cuT( "( " );
		writeFunctionCallRec( separator, result, params... );
		return result;
	}

	template< typename Return, typename OptType, typename ... Params >
	inline Optional< typename details::RemoveOptional< Return >::Type > writeFunctionCall( GlslWriter * writer
		, castor::String const & name
		, Optional< OptType > const & p_param
		, Params const & ... params )
	{
		using NonOptionalReturn = typename details::RemoveOptional< Return >::Type;
		return Optional< NonOptionalReturn >( writeFunctionCall< NonOptionalReturn >( writer
				, name
				, static_cast< OptType const & >( p_param )
				, params... )
			, p_param.isEnabled() );
	}

	//***********************************************************************************************

	template< typename Return, typename Param, typename ... Params >
	inline void writeFunctionHeaderRec( castor::String & separator, Return & result, Param && current, Params && ... params );

	template< typename Return >
	inline void writeFunctionHeaderRec( castor::String & separator, Return & result )
	{
		result.m_value << cuT( "()" );
	}

	template< typename Return, typename Param >
	inline void writeFunctionHeaderRec( castor::String & separator, Return & result, Param && p_last )
	{
		result.m_value << paramToString( separator, p_last ) << cuT( " )" );
	}

	template< typename Return, typename Param, typename ... Params >
	inline void writeFunctionHeaderRec( castor::String & separator, Return & result, Param && current, Params && ... params )
	{
		result.m_value << paramToString( separator, current );
		writeFunctionHeaderRec( separator, result, std::forward< Params >( params )... );
	}

	template< typename Return, typename ... Params >
	inline void writeFunctionHeader( GlslWriter * writer, castor::String const & name, Params && ... params )
	{
		Return result( writer );
		result.m_value << result.m_type << cuT( " " ) << name;
		castor::String separator = cuT( "( " );
		writeFunctionHeaderRec( separator, result, std::forward< Params >( params )... );
		writeLine( *writer, castor::String( result ) );
	}

	template<>
	inline void writeFunctionHeader< void >( GlslWriter * writer, castor::String const & name )
	{
		Void result( writer );
		result.m_value << result.m_type << cuT( " " ) << name << cuT( "()" );
		writeLine( *writer, castor::String( result ) );
	}

	template< typename Return, typename ... Params >
	inline void writeFunctionHeader( GlslWriter & writer, castor::String const & name, Params && ... params )
	{
		writeFunctionHeader< Return >( &writer, name, params... );
	}

	//***********************************************************************************************

	template< typename RetT, typename ... ParamsT >
	Function< RetT, ParamsT... >::Function( GlslWriter * writer, castor::String const & name )
		: m_writer{ writer }
		, m_name{ name }
	{
	}

	template< typename RetT, typename ... ParamsT >
	RetT Function< RetT, ParamsT... >::operator()( ParamsT && ... params )const
	{
		REQUIRE( !m_name.empty() );
		return writeFunctionCall< RetT >( m_writer, m_name, std::forward< ParamsT >( params )... );
	}

	//***********************************************************************************************
}
