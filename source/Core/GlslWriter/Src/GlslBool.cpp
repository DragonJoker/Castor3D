#include "GlslBool.hpp"

using namespace castor;

namespace GLSL
{
	Boolean operator==( Type const & p_a, Type const & p_b )
	{
		Boolean result( p_a.m_writer );
		result.m_value << String( p_a ) << cuT( " == " ) << String( p_b );
		return result;
	}

	Boolean operator!=( Type const & p_a, Type const & p_b )
	{
		Boolean result( p_a.m_writer );
		result.m_value << String( p_a ) << cuT( " != " ) << String( p_b );
		return result;
	}
}
