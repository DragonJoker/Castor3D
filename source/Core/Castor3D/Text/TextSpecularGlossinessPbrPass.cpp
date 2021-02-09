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

		if ( auto block = beginBlock( "pass", file ) )
		{
			result = write( "albedo", pass.getDiffuse(), file )
				&& write( "roughness", pass.getSpecular(), file )
				&& write( "metallic", pass.getGlossiness(), file )
				&& write< Pass >( pass, file );
		}

		return result;
	}
}
