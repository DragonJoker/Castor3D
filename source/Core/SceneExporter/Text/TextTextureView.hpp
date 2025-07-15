/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextTextureView_H___
#define ___CSE_TextTextureView_H___

#include <Castor3D/Material/Texture/TextureView.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< TextureView >
		: public TextWriterT< TextureView >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( TextureView const & obj
			, StringStream & file )override;
	};
}

#endif
