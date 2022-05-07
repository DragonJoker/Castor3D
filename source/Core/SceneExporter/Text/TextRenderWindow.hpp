/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextRenderWindow_H___
#define ___CSE_TextRenderWindow_H___

#include <Castor3D/Render/RenderWindow.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::RenderWindow >
		: public TextWriterT< castor3d::RenderWindow >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::RenderWindow const & material
			, castor::StringStream & file )override;
	};
}

#endif
