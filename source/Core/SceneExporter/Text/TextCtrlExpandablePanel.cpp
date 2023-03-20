#include "TextCtrlExpandablePanel.hpp"

#include "TextCtrlButton.hpp"
#include "TextCtrlControl.hpp"
#include "TextCtrlPanel.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>

namespace castor
{
	using namespace castor3d;

	//*********************************************************************************************

	TextWriter< ExpandablePanelCtrl >::TextWriter( String const & tabs )
		: TextWriterT< ExpandablePanelCtrl >{ tabs }
	{
	}

	bool TextWriter< ExpandablePanelCtrl >::operator()( ExpandablePanelCtrl const & control
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing ExpandablePanelCtrl " ) << control.getName() << std::endl;
		auto result = writeSub< Control >( file, control )
			&& writeNameOpt( file, "expand_caption", control.getExpandCaption(), "+" )
			&& writeNameOpt( file, "retract_caption", control.getRetractCaption(), "-" );

		if ( result )
		{
			if ( auto block{ beginBlock( file, cuT( "header" ) ) } )
			{
				result = TextWriter< PanelCtrl >{ tabs(), true }( *control.getHeader(), file );
			}
		}

		if ( result )
		{
			if ( auto block{ beginBlock( file, cuT( "content" ) ) } )
			{
				result = TextWriter< PanelCtrl >{ tabs(), true }( *control.getContent(), file );
			}
		}

		return result;
	}

	//*********************************************************************************************

	TextWriter< ExpandablePanelStyle >::TextWriter( String const & tabs
		, String const & fontName )
		: TextWriterT< ExpandablePanelStyle >{ tabs }
		, m_fontName{ fontName }
	{
	}

	bool TextWriter< ExpandablePanelStyle >::operator()( ExpandablePanelStyle const & style
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing ExpandablePanelStyle " ) << style.getName() << std::endl;
		auto result = writeSub< ControlStyle >( file, style );

		if ( result )
		{
			if ( auto block{ beginBlock( file, cuT( "header_style" ) ) } )
			{
				result = TextWriter< PanelStyle >{ tabs(), m_fontName }( style.getHeaderStyle(), file );
			}
		}

		if ( result )
		{
			if ( auto block{ beginBlock( file, cuT( "expand_style" ) ) } )
			{
				result = TextWriter< ButtonStyle >{ tabs(), m_fontName }( style.getExpandStyle(), file );
			}
		}

		if ( result )
		{
			if ( auto block{ beginBlock( file, cuT( "content_style" ) ) } )
			{
				result = TextWriter< PanelStyle >{ tabs(), m_fontName }( style.getContentStyle(), file );
			}
		}

		return result;
	}

	//*********************************************************************************************
}
