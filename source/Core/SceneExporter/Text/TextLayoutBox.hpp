/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextLayoutBox_H___
#define ___CSE_TextLayoutBox_H___

#include <Castor3D/Gui/Layout/LayoutBox.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< LayoutBox >
			: public TextWriterT< LayoutBox >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( LayoutBox const & overlay
			, StringStream & file )override;
	};
}

#endif
