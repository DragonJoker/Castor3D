#include "TextTextureConfiguration.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Pass/Component/PassComponentRegister.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< TextureConfiguration >::TextWriter( String const & tabs
		, castor3d::Engine const & engine )
		: TextWriterT< TextureConfiguration >{ tabs, cuT( "TextureConfiguration" ) }
		, m_engine{ engine }
	{
	}

	bool TextWriter< TextureConfiguration >::operator()( TextureConfiguration const & configuration
		, castor::StringStream & file )
	{
		log::info << tabs() << cuT( "Writing TextureConfiguration" ) << std::endl;
		auto result = m_engine.getPassComponentsRegister().writeTextureConfig( configuration
			, tabs()
			, file );

		if ( result )
		{
			result = writeOpt( file, cuT( "invert_y" ), configuration.needsYInversion != 0 );
		}

		return result;
	}
}
