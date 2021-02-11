#include "Castor3D/Text/TextPhongPass.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Text/TextPass.hpp"

#include <CastorUtils/Data/Text/TextRgbColour.hpp>

using namespace castor3d;

namespace castor
{
	TextWriter< PhongPass >::TextWriter( String const & tabs
		, String const & subfolder )
		: TextWriterT< PhongPass >{ tabs }
		, m_subfolder{ subfolder }
	{
	}

	bool TextWriter< PhongPass >::operator()( PhongPass const & pass
		, TextFile & file )
	{
		log::info << tabs() << cuT( "Writing PhongPass " ) << std::endl;
		bool result = false;

		if ( auto block{ beginBlock( file, "pass" ) } )
		{
			result = writeNamedSub( file, "diffuse", pass.getDiffuse() )
				&& writeNamedSub( file, "specular", pass.getSpecular() )
				&& write( file, "ambient", pass.getAmbient() )
				&& write( file, "shininess", pass.getShininess() )
				&& writeSub< Pass >( file, pass, m_subfolder );
		}

		return result;
	}
}
