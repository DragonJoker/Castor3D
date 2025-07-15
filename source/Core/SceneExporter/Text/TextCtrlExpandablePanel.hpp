/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextExpandablePanelCtrl_H___
#define ___CSE_TextExpandablePanelCtrl_H___

#include <Castor3D/Gui/Controls/CtrlExpandablePanel.hpp>
#include <Castor3D/Gui/Theme/StyleExpandablePanel.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< ExpandablePanelCtrl >
		: public TextWriterT< ExpandablePanelCtrl >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( ExpandablePanelCtrl const & overlay
			, StringStream & file )override;
	};

	template<>
	class TextWriter< ExpandablePanelStyle >
		: public TextWriterT< ExpandablePanelStyle >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & fontName );
		bool operator()( ExpandablePanelStyle const & overlay
			, StringStream & file )override;

	private:
		String m_fontName;
	};
}

#endif
