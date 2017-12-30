#include "GlslBool.hpp"

using namespace castor;

namespace glsl
{
	Boolean operator==( Type const & lhs, Type const & rhs )
	{
		Boolean result( lhs.m_writer );
		result.m_value << String( lhs ) << cuT( " == " ) << String( rhs );
		return result;
	}

	Boolean operator!=( Type const & lhs, Type const & rhs )
	{
		Boolean result( lhs.m_writer );
		result.m_value << String( lhs ) << cuT( " != " ) << String( rhs );
		return result;
	}

	Boolean operator||( Boolean const & lhs, Boolean const & rhs )
	{
		Boolean result( lhs.m_writer );
		result.m_value << String( lhs ) << cuT( " || " ) << String( rhs );
		return result;
	}

	Boolean operator&&( Boolean const & lhs, Boolean const & rhs )
	{
		Boolean result( lhs.m_writer );
		result.m_value << String( lhs ) << cuT( " && " ) << String( rhs );
		return result;
	}
}
