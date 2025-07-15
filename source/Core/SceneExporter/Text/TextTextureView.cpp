#include "TextTextureView.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

namespace c3d
{
	TextWriter< TextureView >::TextWriter( String const & tabs )
		: TextWriterT< TextureView >{ tabs }
	{
	}

	bool TextWriter< TextureView >::operator()( TextureView const & obj
		, StringStream & file )
	{
		return write( file, cuT( "" ), obj.toString() );
	}
}
