namespace GLSL
{
	//*****************************************************************************************

	template< typename TypeT >
	InParam< TypeT >::InParam( GlslWriter * writer
		, castor::String const & name )
		: TypeT( writer, name )
	{
		TypeT::m_type = cuT( "in " ) + TypeT::m_type;
	}

	template< typename TypeT >
	InParam< TypeT >::InParam( TypeT const & other )
		: TypeT( other.m_writer, toString( other ) )
	{
		TypeT::m_type = cuT( "in " ) + TypeT::m_type;
	}

	template< typename TypeT >
	template< typename T >
	InParam< TypeT > InParam< TypeT >::operator=( T const & rhs )
	{
		TypeT::m_writer->writeAssign( *this, rhs );
		return *this;
	}

	//*****************************************************************************************

	template< typename TypeT >
	OutParam< TypeT >::OutParam( GlslWriter * writer
		, castor::String const & name )
		: TypeT( writer, name )
	{
		TypeT::m_type = cuT( "out " ) + TypeT::m_type;
	}

	template< typename TypeT >
	OutParam< TypeT >::OutParam( TypeT const & other )
		: TypeT( other.m_writer, toString( other ) )
	{
		TypeT::m_type = cuT( "in " ) + TypeT::m_type;
	}

	template< typename TypeT >
	template< typename T >
	OutParam< TypeT > OutParam< TypeT >::operator=( T const & rhs )
	{
		TypeT::m_writer->writeAssign( *this, rhs );
		return *this;
	}

	//*****************************************************************************************

	template< typename TypeT >
	InOutParam< TypeT >::InOutParam( GlslWriter * writer
		, castor::String const & name )
		: TypeT( writer, name )
	{
		TypeT::m_type = cuT( "inout " ) + TypeT::m_type;
	}

	template< typename TypeT >
	InOutParam< TypeT >::InOutParam( TypeT const & other )
		: TypeT( other.m_writer, toString( other ) )
	{
		TypeT::m_type = cuT( "in " ) + TypeT::m_type;
	}

	template< typename TypeT >
	template< typename T >
	InOutParam< TypeT > InOutParam< TypeT >::operator=( T const & rhs )
	{
		TypeT::m_writer->writeAssign( *this, rhs );
		return *this;
	}

	//*****************************************************************************************

	template< typename TypeT >
	InArrayParam< TypeT >::InArrayParam( GlslWriter * writer
		, castor::String const & name
		, uint32_t count )
		: Array< TypeT >( writer, name )
	{
		TypeT::m_type = cuT( "in " ) + TypeT::m_type + cuT( "[" ) + toString( count ) + cuT( "]" );
	}

	template< typename TypeT >
	InArrayParam< TypeT >::InArrayParam( Array< TypeT > const & other )
		: Array< TypeT >( other.m_writer, toString( other ), other.m_dimension )
	{
		TypeT::m_type = cuT( "in " ) + TypeT::m_type + cuT( "[" ) + toString( other.m_dimension ) + cuT( "]" );
	}

	template< typename TypeT >
	template< typename T >
	InArrayParam< TypeT > InArrayParam< TypeT >::operator=( T const & rhs )
	{
		TypeT::m_writer->writeAssign( *this, rhs );
		return *this;
	}

	//*****************************************************************************************

	template< typename TypeT >
	OutArrayParam< TypeT >::OutArrayParam( GlslWriter * writer
		, castor::String const & name
		, uint32_t count )
		: Array< TypeT >( writer, name )
	{
		TypeT::m_type = cuT( "out " ) + TypeT::m_type + cuT( "[" ) + toString( count ) + cuT( "]" );
	}

	template< typename TypeT >
	OutArrayParam< TypeT >::OutArrayParam( Array< TypeT > const & other )
		: Array< TypeT >( other.m_writer, toString( other ), other.m_dimension )
	{
		TypeT::m_type = cuT( "in " ) + TypeT::m_type + cuT( "[" ) + toString( other.m_dimension ) + cuT( "]" );
	}

	template< typename TypeT >
	template< typename T >
	OutArrayParam< TypeT > OutArrayParam< TypeT >::operator=( T const & rhs )
	{
		TypeT::m_writer->writeAssign( *this, rhs );
		return *this;
	}

	//*****************************************************************************************

	template< typename TypeT >
	InOutArrayParam< TypeT >::InOutArrayParam( GlslWriter * writer
		, castor::String const & name
		, uint32_t count )
		: Array< TypeT >( writer, name )
	{
		TypeT::m_type = cuT( "inout " ) + TypeT::m_type + cuT( "[" ) + toString( count ) + cuT( "]" );
	}

	template< typename TypeT >
	InOutArrayParam< TypeT >::InOutArrayParam( Array< TypeT > const & other )
		: Array< TypeT >( other.m_writer, toString( other ), other.m_dimension )
	{
		TypeT::m_type = cuT( "in " ) + TypeT::m_type + cuT( "[" ) + toString( other.m_dimension ) + cuT( "]" );
	}

	template< typename TypeT >
	template< typename T >
	InOutArrayParam< TypeT > InOutArrayParam< TypeT >::operator=( T const & rhs )
	{
		TypeT::m_writer->writeAssign( *this, rhs );
		return *this;
	}

	//*****************************************************************************************
}
