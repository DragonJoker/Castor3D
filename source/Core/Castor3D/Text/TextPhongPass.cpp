#include "Castor3D/Text/TextPhongPass.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Text/TextPass.hpp"

#include <CastorUtils/Data/Text/TextRgbColour.hpp>

using namespace castor3d;

namespace castor
{
	TextWriter< PhongPass >::TextWriter( String const & tabs )
		: TextWriterT< PhongPass >{ tabs }
	{
	}

	bool TextWriter< PhongPass >::operator()( PhongPass const & pass
		, TextFile & file )
	{
		log::info << tabs() << cuT( "Writing PhongPass " ) << std::endl;
		bool result = false;

		if ( auto block = beginBlock( "pass", file ) )
		{
			result = write( "diffuse", pass.getDiffuse(), file )
				&& write( "specular", pass.getSpecular(), file )
				&& write( "ambient", pass.getAmbient(), file )
				&& write( "shininess", pass.getShininess(), file )
				&& write< Pass >( pass, file );
		}

		return result;
	}
}
