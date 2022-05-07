#include "TextCamera.hpp"

#include "TextHdrConfig.hpp"
#include "TextViewport.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< Camera >::TextWriter( String const & tabs )
		: TextWriterT< Camera >{ tabs }
	{
	}

	bool TextWriter< Camera >::operator()( Camera const & camera
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing Camera " ) << camera.getName() << std::endl;
		bool result = false;
		
		if ( auto block{ beginBlock( file, cuT( "camera" ), camera.getName() ) } )
		{
			result = writeName( file, "parent", camera.getParent()->getName() )
				&& writeSub( file, camera.getViewport() )
				&& writeSub( file, camera.getHdrConfig() );
		}

		return result;
	}
}
