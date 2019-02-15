#include "RenderedObject.hpp"

using namespace castor;

namespace castor3d
{
	RenderedObject::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< RenderedObject >{ p_tabs }
	{
	}

	bool RenderedObject::TextWriter::operator()( RenderedObject const & p_object, TextFile & p_file )
	{
		bool result{ true };

		if ( !p_object.isShadowCaster() )
		{
			result = p_file.writeText( m_tabs + cuT( "cast_shadows false\n" ) ) > 0;
			castor::TextWriter< RenderedObject >::checkError( result, "Object shadow caster status" );
		}

		if ( !p_object.isShadowReceiver() )
		{
			result = p_file.writeText( m_tabs + cuT( "receives_shadows false\n" ) ) > 0;
			castor::TextWriter< RenderedObject >::checkError( result, "Object shadow receiver status" );
		}

		return result;
	}
}
