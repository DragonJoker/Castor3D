/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextExpandablePanelCtrl_H___
#define ___CSE_TextExpandablePanelCtrl_H___

#include <Castor3D/Gui/Controls/CtrlExpandablePanel.hpp>
#include <Castor3D/Gui/Theme/StyleExpandablePanel.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::ExpandablePanelCtrl >
		: public TextWriterT< castor3d::ExpandablePanelCtrl >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::ExpandablePanelCtrl const & overlay
			, castor::StringStream & file )override;
	};

	template<>
	class TextWriter< castor3d::ExpandablePanelStyle >
		: public TextWriterT< castor3d::ExpandablePanelStyle >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & fontName );
		bool operator()( castor3d::ExpandablePanelStyle const & overlay
			, castor::StringStream & file )override;

	private:
		String m_fontName;
	};
}

#endif
