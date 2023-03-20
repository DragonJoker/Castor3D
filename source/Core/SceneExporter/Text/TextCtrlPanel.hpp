/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextPanelCtrl_H___
#define ___CSE_TextPanelCtrl_H___

#include <Castor3D/Gui/Controls/CtrlPanel.hpp>
#include <Castor3D/Gui/Theme/StylePanel.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::PanelCtrl >
		: public TextWriterT< castor3d::PanelCtrl >
	{
	public:
		explicit TextWriter( String const & tabs
			, bool customStyle = false );
		bool operator()( castor3d::PanelCtrl const & object
			, castor::StringStream & file )override;

	private:
		bool m_customStyle;
	};

	template<>
	class TextWriter< castor3d::PanelStyle >
		: public TextWriterT< castor3d::PanelStyle >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & fontName );
		bool operator()( castor3d::PanelStyle const & object
			, castor::StringStream & file )override;

	private:
		String m_fontName;
	};
}

#endif
