/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextImageBackground_H___
#define ___C3D_TextImageBackground_H___

#include "Castor3D/Scene/Background/Image.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::ImageBackground >
		: public TextWriterT< castor3d::ImageBackground >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs
			, castor::Path const & folder );
		C3D_API bool operator()( castor3d::ImageBackground const & overlay
			, castor::StringStream & file );

	private:
		castor::Path const & m_folder;
	};
}

#endif
