#include "GlslBool.hpp"

namespace GlRender
{
	namespace GLSL
	{
		GlslBool operator==( Type const & p_a, Type const & p_b )
		{
			GlslBool l_return( p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " == " ) << Castor::String( p_b );
			return l_return;
		}

		GlslBool operator!=( Type const & p_a, Type const & p_b )
		{
			GlslBool l_return( p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " != " ) << Castor::String( p_b );
			return l_return;
		}
	}
}
