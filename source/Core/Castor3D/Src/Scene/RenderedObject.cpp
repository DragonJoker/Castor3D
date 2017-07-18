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
		bool result{ true };

		if ( !p_object.IsShadowCaster() )
		{
			result = p_file.WriteText( m_tabs + cuT( "cast_shadows false\n" ) ) > 0;
			Castor::TextWriter< RenderedObject >::CheckError( result, "Object shadow caster status" );
		}

		if ( !p_object.IsShadowReceiver() )
		{
			result = p_file.WriteText( m_tabs + cuT( "receives_shadows false\n" ) ) > 0;
			Castor::TextWriter< RenderedObject >::CheckError( result, "Object shadow receiver status" );
		}

		return result;
	}
}
