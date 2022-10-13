#include "TextTextureConfiguration.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Pass/Component/PassComponentRegister.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< TextureConfiguration >::TextWriter( String const & tabs
		, castor3d::TextureUnit const & parent
		, bool isPbr )
		: TextWriterT< TextureConfiguration >{ tabs, cuT( "TextureConfiguration" ) }
		, m_parent{ parent }
		, m_isPbr{ isPbr }
	{
	}

	bool TextWriter< TextureConfiguration >::operator()( TextureConfiguration const & configuration
		, castor::StringStream & file )
	{
		log::info << tabs() << cuT( "Writing TextureConfiguration" ) << std::endl;
		auto result = m_parent.getEngine()->getPassComponentsRegister().writeTextureConfig( configuration
			, tabs()
			, file );

		if ( result )
		{
			result = writeOpt( file, cuT( "invert_y" ), configuration.needsYInversion != 0 );
		}

		return result;
	}
}
