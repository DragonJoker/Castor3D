/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextListBoxCtrl_H___
#define ___CSE_TextListBoxCtrl_H___

#include <Castor3D/Gui/Controls/CtrlListBox.hpp>
#include <Castor3D/Gui/Theme/StyleListBox.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< ListBoxCtrl >
		: public TextWriterT< ListBoxCtrl >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( ListBoxCtrl const & overlay
			, StringStream & file )override;
	};

	template<>
	class TextWriter< ListBoxStyle >
		: public TextWriterT< ListBoxStyle >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & fontName );
		bool operator()( ListBoxStyle const & overlay
			, StringStream & file )override;

	private:
		String m_fontName;
	};
}

#endif
