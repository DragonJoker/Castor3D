#include "TextFog.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

namespace c3d
{
	TextWriter< Fog >::TextWriter( String const & tabs )
		: TextWriterT< Fog >{ tabs }
	{
	}

	bool TextWriter< Fog >::operator()( Fog const & config
		, StringStream & file )
	{
		bool result{ true };

		if ( config.getType() != FogType::eDisabled )
		{
			log::info << tabs() << cuT( "Write Fog" ) << std::endl;
			result = write( file, cuT( "fog_type" ), getName( config.getType() ) )
				&& write( file, cuT( "fog_density " ), config.getDensity() );
		}

		return result;
	}
}
