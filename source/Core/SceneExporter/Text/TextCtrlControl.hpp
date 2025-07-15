/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextControl_H___
#define ___CSE_TextControl_H___

#include <Castor3D/Gui/Controls/CtrlControl.hpp>
#include <Castor3D/Gui/Theme/StyleControl.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< Control >
		: public TextWriterT< Control >
	{
	public:
		explicit TextWriter( String const & tabs
			, bool customStyle = false );
		bool operator()( Control const & overlay
			, StringStream & file )override;

	private:
		bool m_customStyle;
	};

	template<>
	class TextWriter< ControlStyle >
		: public TextWriterT< ControlStyle >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( ControlStyle const & overlay
			, StringStream & file )override;

	private:
		String m_fontName;
	};
}

#endif
