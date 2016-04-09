#include "GlslBool.hpp"

using namespace Castor;

namespace GLSL
{
	GlslBool operator==( Type const & p_a, Type const & p_b )
	{
		GlslBool l_return( p_a.m_writer );
		l_return.m_value << String( p_a ) << cuT( " == " ) << String( p_b );
		return l_return;
	}

	GlslBool operator!=( Type const & p_a, Type const & p_b )
	{
		GlslBool l_return( p_a.m_writer );
		l_return.m_value << String( p_a ) << cuT( " != " ) << String( p_b );
		return l_return;
	}
}
