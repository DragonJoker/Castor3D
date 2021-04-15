#include "Castor3D/Text/TextMetallicRoughnessPbrPass.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Text/TextPass.hpp"

#include <CastorUtils/Data/Text/TextRgbColour.hpp>

using namespace castor3d;

namespace castor
{
	TextWriter< MetallicRoughnessPbrPass >::TextWriter( String const & tabs
		, Path const & folder
		, String const & subfolder )
		: TextWriterT< MetallicRoughnessPbrPass >{ tabs }
		, m_folder{ folder }
		, m_subfolder{ subfolder }
	{
	}

	bool TextWriter< MetallicRoughnessPbrPass >::operator()( MetallicRoughnessPbrPass const & pass
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing MetallicRoughnessPbrPass " ) << std::endl;
		bool result = false;

		if ( auto block{ beginBlock( file, "pass" ) } )
		{
			result = writeNamedSub( file, "albedo", pass.getAlbedo() )
				&& write( file, "roughness", pass.getRoughness() )
				&& write( file, "metallic", pass.getMetallic() )
				&& writeSub< Pass >( file, pass, m_folder, m_subfolder );
		}

		return result;
	}
}
