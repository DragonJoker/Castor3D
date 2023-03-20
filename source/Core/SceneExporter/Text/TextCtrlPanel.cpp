#include "TextCtrlPanel.hpp"

#include "TextCtrlControl.hpp"
#include "TextCtrlLayoutControl.hpp"
#include "TextStylesHolder.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

namespace castor
{
	using namespace castor3d;

	//*********************************************************************************************

	TextWriter< PanelCtrl >::TextWriter( String const & tabs
		, bool customStyle )
		: TextWriterT< PanelCtrl >{ tabs }
		, m_customStyle{ customStyle }
	{
	}

	bool TextWriter< PanelCtrl >::operator()( PanelCtrl const & control
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing PanelCtrl " ) << control.getName() << std::endl;

		return TextWriter< LayoutControl >{ tabs(), m_customStyle }( control, file );
	}

	//*********************************************************************************************

	TextWriter< PanelStyle >::TextWriter( String const & tabs
		, String const & fontName )
		: TextWriterT< PanelStyle >{ tabs }
		, m_fontName{ fontName }
	{
	}

	bool TextWriter< PanelStyle >::operator()( PanelStyle const & style
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing PanelStyle " ) << style.getName() << std::endl;
		TextWriter< StylesHolder > writer{ tabs()
			, style.hasScene() ? &style.getScene() : nullptr
			, m_fontName };
		return writer( style, file )
			&& writeSub< ControlStyle >( file, style );
	}

	//*********************************************************************************************
}
