#include "TextCtrlFrame.hpp"

#include "TextCtrlControl.hpp"
#include "TextCtrlPanel.hpp"
#include "TextCtrlStatic.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>

namespace castor
{
	using namespace castor3d;

	//*********************************************************************************************

	TextWriter< FrameCtrl >::TextWriter( String const & tabs )
		: TextWriterT< FrameCtrl >{ tabs }
	{
	}

	bool TextWriter< FrameCtrl >::operator()( FrameCtrl const & control
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing FrameCtrl " ) << control.getName() << std::endl;
		auto result = writeSub< Control >( file, control )
			&& writeName( file, "header_caption", control.getHeaderCaption() )
			&& writeOpt( file, "header_horizontal_align", getName( control.getHeaderHAlign() ), getName( HAlign::eLeft ) )
			&& writeOpt( file, "header_vertical_align", getName( control.getHeaderVAlign() ), getName( VAlign::eTop ) );

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

	TextWriter< FrameStyle >::TextWriter( String const & tabs
		, String const & fontName )
		: TextWriterT< FrameStyle >{ tabs }
		, m_fontName{ fontName }
	{
	}

	bool TextWriter< FrameStyle >::operator()( FrameStyle const & style
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing FrameStyle " ) << style.getName() << std::endl;
		return writeSub< ControlStyle >( file, style )
			&& writeNameOpt( file, "header_font", style.getHeaderFontName(), m_fontName )
			&& writeName( file, "header_text_material", style.getHeaderTextMaterial()->getName() );
	}

	//*********************************************************************************************
}
