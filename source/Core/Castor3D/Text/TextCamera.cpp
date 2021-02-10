#include "Castor3D/Text/TextCamera.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Text/TextHdrConfig.hpp"
#include "Castor3D/Text/TextViewport.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< Camera >::TextWriter( String const & tabs )
		: TextWriterT< Camera >{ tabs }
	{
	}

	bool TextWriter< Camera >::operator()( Camera const & camera, TextFile & file )
	{
		log::info << tabs() << cuT( "Writing Camera " ) << camera.getName() << std::endl;
		bool result = false;
		
		if ( auto block = beginBlock( file, cuT( "camera" ), camera.getName() ) )
		{
			result = writeName( file, "parent", camera.getParent()->getName() )
				&& write( file, camera.getViewport() )
				&& write( file, camera.getHdrConfig() );
		}

		return result;
	}
}
