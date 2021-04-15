/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextTextureView_H___
#define ___C3D_TextTextureView_H___

#include "Castor3D/Material/Texture/TextureView.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::TextureView >
		: public TextWriterT< castor3d::TextureView >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs );
		C3D_API bool operator()( castor3d::TextureView const & obj
			, StringStream & file )override;
	};
}

#endif
