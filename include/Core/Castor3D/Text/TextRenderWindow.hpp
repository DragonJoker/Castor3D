/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextRenderWindow_H___
#define ___C3D_TextRenderWindow_H___

#include "Castor3D/Render/RenderWindow.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::RenderWindow >
		: public TextWriterT< castor3d::RenderWindow >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs );
		C3D_API bool operator()( castor3d::RenderWindow const & material
			, castor::StringStream & file )override;
	};
}

#endif
