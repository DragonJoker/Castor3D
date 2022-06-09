#include "Castor3D/Text/TextRsmConfig.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

namespace castor
{
	using namespace castor3d;

	TextWriter< RsmConfig >::TextWriter( String const & tabs )
		: castor::TextWriterT< RsmConfig >{ tabs }
	{
	}

	bool TextWriter< RsmConfig >::operator()( RsmConfig const & object
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing RsmConfig" ) << std::endl;
		auto result = false;

		if ( auto block{ beginBlock( file, cuT( "rsm_config" ) ) } )
		{
			result = write( file, cuT( "intensity" ), object.intensity )
				&& write( file, cuT( "max_radius" ), object.maxRadius )
				&& write( file, cuT( "sample_count" ), object.sampleCount );
		}

		return result;
	}
}
