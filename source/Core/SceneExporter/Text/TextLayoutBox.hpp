/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextLayoutBox_H___
#define ___CSE_TextLayoutBox_H___

#include <Castor3D/Gui/Layout/LayoutBox.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::LayoutBox >
			: public TextWriterT< castor3d::LayoutBox >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::LayoutBox const & overlay
			, castor::StringStream & file )override;
	};
}

#endif
