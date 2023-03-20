/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextLayoutControl_H___
#define ___CSE_TextLayoutControl_H___

#include <Castor3D/Gui/Controls/CtrlLayoutControl.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::LayoutControl >
		: public TextWriterT< castor3d::LayoutControl >
	{
	public:
		explicit TextWriter( String const & tabs
			, bool customStyle = false );
		bool operator()( castor3d::LayoutControl const & overlay
			, StringStream & file )override;

	private:
		bool m_customStyle;
	};

	bool writeControl( TextWriterBase & writer
		, castor3d::Control const & control
		, StringStream & file
		, String const & prefix = {} );
}

#endif
