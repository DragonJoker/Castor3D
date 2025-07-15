/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextPanelCtrl_H___
#define ___CSE_TextPanelCtrl_H___

#include <Castor3D/Gui/Controls/CtrlPanel.hpp>
#include <Castor3D/Gui/Theme/StylePanel.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< PanelCtrl >
		: public TextWriterT< PanelCtrl >
	{
	public:
		explicit TextWriter( String const & tabs
			, bool customStyle = false );
		bool operator()( PanelCtrl const & object
			, StringStream & file )override;

	private:
		bool m_customStyle;
	};

	template<>
	class TextWriter< PanelStyle >
		: public TextWriterT< PanelStyle >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & fontName );
		bool operator()( PanelStyle const & object
			, StringStream & file )override;

	private:
		String m_fontName;
	};
}

#endif
