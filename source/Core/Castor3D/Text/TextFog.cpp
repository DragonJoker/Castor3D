#include "Castor3D/Text/TextFog.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< Fog >::TextWriter( castor::String const & tabs )
		: castor::TextWriterT< Fog >{ tabs }
	{
	}

	bool TextWriter< Fog >::operator()( Fog const & config
		, TextFile & file )
	{
		bool result{ true };

		if ( config.getType() != FogType::eDisabled )
		{
			log::info << cuT( "Write Fog" ) << std::endl;
			result = write( cuT( "fog_type" ), castor3d::getName( config.getType() ), file )
				&& write( cuT( "fog_density " ), config.getDensity(), file );
		}

		return result;
	}
}
