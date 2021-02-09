#include "Castor3D/Text/TextRsmConfig.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< RsmConfig >::TextWriter( String const & tabs )
		: castor::TextWriterT< RsmConfig >{ tabs }
	{
	}

	bool TextWriter< RsmConfig >::operator()( RsmConfig const & object, TextFile & file )
	{
		log::info << tabs() << cuT( "Writing RsmConfig" ) << std::endl;
		auto result = false;

		if ( auto block = beginBlock( cuT( "lpv_config" ), file ) )
		{
			result = write( cuT( "intensity" ), object.intensity, file )
				&& write( cuT( "max_radius" ), object.maxRadius, file )
				&& write( cuT( "sample_count" ), object.sampleCount, file );
		}

		return result;
	}
}
