#include "Castor3D/Text/TextTextureView.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< TextureView >::TextWriter( String const & tabs )
		: TextWriterT< TextureView >{ tabs }
	{
	}

	bool TextWriter< TextureView >::operator()( TextureView const & obj
		, StringStream & file )
	{
		return write( file, "", obj.toString() );
	}
}
