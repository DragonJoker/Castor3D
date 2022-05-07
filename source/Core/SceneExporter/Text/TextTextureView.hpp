/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextTextureView_H___
#define ___CSE_TextTextureView_H___

#include <Castor3D/Material/Texture/TextureView.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::TextureView >
		: public TextWriterT< castor3d::TextureView >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::TextureView const & obj
			, StringStream & file )override;
	};
}

#endif
