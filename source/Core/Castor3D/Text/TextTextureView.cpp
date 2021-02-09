#include "Castor3D/Text/TextTextureView.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< TextureView >::TextWriter( String const & tabs )
		: TextWriterT< TextureView >{ tabs }
	{
	}

	bool TextWriter< TextureView >::operator()( TextureView const & obj, TextFile & file )
	{
		return write( "", obj.toString(), file );
	}
}
