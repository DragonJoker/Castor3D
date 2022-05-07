#include "TextFog.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< Fog >::TextWriter( castor::String const & tabs )
		: castor::TextWriterT< Fog >{ tabs }
	{
	}

	bool TextWriter< Fog >::operator()( Fog const & config
		, StringStream & file )
	{
		bool result{ true };

		if ( config.getType() != FogType::eDisabled )
		{
			log::info << tabs() << cuT( "Write Fog" ) << std::endl;
			result = write( file, cuT( "fog_type" ), castor3d::getName( config.getType() ) )
				&& write( file, cuT( "fog_density " ), config.getDensity() );
		}

		return result;
	}
}
