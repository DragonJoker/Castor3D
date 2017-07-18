#include "GlslBool.hpp"

using namespace Castor;

namespace GLSL
{
	Boolean operator==( Type const & p_a, Type const & p_b )
	{
		Boolean l_result( p_a.m_writer );
		l_result.m_value << String( p_a ) << cuT( " == " ) << String( p_b );
		return l_result;
	}

	Boolean operator!=( Type const & p_a, Type const & p_b )
	{
		Boolean l_result( p_a.m_writer );
		l_result.m_value << String( p_a ) << cuT( " != " ) << String( p_b );
		return l_result;
	}
}
