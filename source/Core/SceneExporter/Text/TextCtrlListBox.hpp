/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextListBoxCtrl_H___
#define ___CSE_TextListBoxCtrl_H___

#include <Castor3D/Gui/Controls/CtrlListBox.hpp>
#include <Castor3D/Gui/Theme/StyleListBox.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::ListBoxCtrl >
		: public TextWriterT< castor3d::ListBoxCtrl >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::ListBoxCtrl const & overlay
			, castor::StringStream & file )override;
	};

	template<>
	class TextWriter< castor3d::ListBoxStyle >
		: public TextWriterT< castor3d::ListBoxStyle >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & fontName );
		bool operator()( castor3d::ListBoxStyle const & overlay
			, castor::StringStream & file )override;

	private:
		String m_fontName;
	};
}

#endif
