namespace GLSL
{
	//*****************************************************************************************

	template< typename TypeT >
	InParam< TypeT >::InParam( GlslWriter * p_writer, Castor::String const & p_name )
		: TypeT( p_writer, p_name )
	{
		TypeT::m_type = cuT( "in " ) + TypeT::m_type;
	}

	template< typename TypeT >
	InParam< TypeT >::InParam( TypeT const & p_other )
		: TypeT( p_other.m_writer, ToString( p_other ) )
	{
		TypeT::m_type = cuT( "in " ) + TypeT::m_type;
	}

	//*****************************************************************************************

	template< typename TypeT >
	OutParam< TypeT >::OutParam( GlslWriter * p_writer, Castor::String const & p_name )
		: TypeT( p_writer, p_name )
	{
		TypeT::m_type = cuT( "out " ) + TypeT::m_type;
	}

	template< typename TypeT >
	OutParam< TypeT >::OutParam( TypeT const & p_other )
		: TypeT( p_other.m_writer, ToString( p_other ) )
	{
		TypeT::m_type = cuT( "in " ) + TypeT::m_type;
	}

	template< typename TypeT >
	template< typename T >
	OutParam< TypeT > OutParam< TypeT >::operator=( T const & p_rhs )
	{
		TypeT::m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	//*****************************************************************************************

	template< typename TypeT >
	InOutParam< TypeT >::InOutParam( GlslWriter * p_writer, Castor::String const & p_name )
		: TypeT( p_writer, p_name )
	{
		TypeT::m_type = cuT( "inout " ) + TypeT::m_type;
	}

	template< typename TypeT >
	InOutParam< TypeT >::InOutParam( TypeT const & p_other )
		: TypeT( p_other.m_writer, ToString( p_other ) )
	{
		TypeT::m_type = cuT( "in " ) + TypeT::m_type;
	}

	template< typename TypeT >
	template< typename T >
	InOutParam< TypeT > InOutParam< TypeT >::operator=( T const & p_rhs )
	{
		TypeT::m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	//*****************************************************************************************
}
