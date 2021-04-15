#include "Castor3D/Text/TextSpecularGlossinessPbrPass.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Text/TextPass.hpp"

#include <CastorUtils/Data/Text/TextRgbColour.hpp>

using namespace castor3d;

namespace castor
{
	TextWriter< SpecularGlossinessPbrPass >::TextWriter( String const & tabs
		, Path const & folder
		, String const & subfolder )
		: TextWriterT< SpecularGlossinessPbrPass >{ tabs }
		, m_folder{ folder }
		, m_subfolder{ subfolder }
	{
	}

	bool TextWriter< SpecularGlossinessPbrPass >::operator()( SpecularGlossinessPbrPass const & pass
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing SpecularGlossinessPbrPass " ) << std::endl;
		bool result = false;

		if ( auto block{ beginBlock( file, "pass" ) } )
		{
			result = writeNamedSub( file, "albedo", pass.getDiffuse() )
				&& writeNamedSub( file, "specular", pass.getSpecular() )
				&& write( file, "glossiness", pass.getGlossiness() )
				&& writeSub< Pass >( file, pass, m_folder, m_subfolder );
		}

		return result;
	}
}
