/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextRenderWindow_H___
#define ___CSE_TextRenderWindow_H___

#include <Castor3D/Render/RenderWindow.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< RenderWindow >
		: public TextWriterT< RenderWindow >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( RenderWindow const & material
			, StringStream & file )override;
	};
}

#endif
