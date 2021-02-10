#include "Castor3D/Text/TextSpecularGlossinessPbrPass.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Text/TextPass.hpp"

#include <CastorUtils/Data/Text/TextRgbColour.hpp>

using namespace castor3d;

namespace castor
{
	TextWriter< SpecularGlossinessPbrPass >::TextWriter( String const & tabs )
		: TextWriterT< SpecularGlossinessPbrPass >{ tabs }
	{
	}

	bool TextWriter< SpecularGlossinessPbrPass >::operator()( SpecularGlossinessPbrPass const & pass
		, TextFile & file )
	{
		log::info << tabs() << cuT( "Writing SpecularGlossinessPbrPass " ) << std::endl;
		bool result = false;

		if ( auto block = beginBlock( file, "pass" ) )
		{
			result = write( file, "albedo", pass.getDiffuse() )
				&& write( file, "roughness", pass.getSpecular() )
				&& write( file, "metallic", pass.getGlossiness() )
				&& write< Pass >( file, pass );
		}

		return result;
	}
}
