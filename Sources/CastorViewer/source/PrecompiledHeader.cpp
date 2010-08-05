#include "PrecompiledHeader.h"

#if C3D_UNICODE

namespace CastorViewer
{
	wxString makeWxString( String const & p_in )
	{
		return wxString( p_in );
	}

	String makeString( wxString const & p_in )
	{
		return String( p_in.wc_str() );
	}
}

#else

namespace CastorViewer
{
	wxString makeWxString( String const & p_in )
	{
		return wxString( p_in );
	}

	String makeString( wxString const & p_in )
	{
		return String( p_in.char_str() );
	}
}

#endif
