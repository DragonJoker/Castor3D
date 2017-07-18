#include "RenderedObject.hpp"

using namespace Castor;

namespace Castor3D
{
	RenderedObject::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< RenderedObject >{ p_tabs }
	{
	}

	bool RenderedObject::TextWriter::operator()( RenderedObject const & p_object, TextFile & p_file )
	{
		bool l_result{ true };

		if ( !p_object.IsShadowCaster() )
		{
			l_result = p_file.WriteText( m_tabs + cuT( "cast_shadows false\n" ) ) > 0;
			Castor::TextWriter< RenderedObject >::CheckError( l_result, "Object shadow caster status" );
		}

		if ( !p_object.IsShadowReceiver() )
		{
			l_result = p_file.WriteText( m_tabs + cuT( "receives_shadows false\n" ) ) > 0;
			Castor::TextWriter< RenderedObject >::CheckError( l_result, "Object shadow receiver status" );
		}

		return l_result;
	}
}
