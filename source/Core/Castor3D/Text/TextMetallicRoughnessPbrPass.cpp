#include "Castor3D/Text/TextMetallicRoughnessPbrPass.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Text/TextPass.hpp"

#include <CastorUtils/Data/Text/TextRgbColour.hpp>

using namespace castor3d;

namespace castor
{
	TextWriter< MetallicRoughnessPbrPass >::TextWriter( String const & tabs )
		: TextWriterT< MetallicRoughnessPbrPass >{ tabs }
	{
	}

	bool TextWriter< MetallicRoughnessPbrPass >::operator()( MetallicRoughnessPbrPass const & pass
		, TextFile & file )
	{
		log::info << tabs() << cuT( "Writing MetallicRoughnessPbrPass " ) << std::endl;
		bool result = false;

		if ( auto block{ beginBlock( file, "pass" ) } )
		{
			result = write( file, "albedo", pass.getAlbedo() )
				&& write( file, "roughness", pass.getRoughness() )
				&& write( file, "metallic", pass.getMetallic() )
				&& write< Pass >( file, pass );
		}

		return result;
	}
}
