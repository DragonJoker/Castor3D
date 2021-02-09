#include "Castor3D/Text/TextLpvConfig.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< LpvConfig >::TextWriter( String const & tabs )
		: TextWriterT< LpvConfig >{ tabs }
	{
	}

	bool TextWriter< LpvConfig >::operator()( LpvConfig const & object, TextFile & file )
	{
		log::info << cuT( "Writing LpvConfig" ) << std::endl;
		bool result{ false };

		if ( beginBlock( cuT( "lpv_config" ), file ) )
		{
			result = write( cuT( "texel_area_modifier" ), object.texelAreaModifier, file );
		}

		return result;
	}
}
