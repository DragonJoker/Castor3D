#include "TextCtrlStatic.hpp"

#include "TextCtrlControl.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>

namespace castor
{
	using namespace castor3d;

	//*********************************************************************************************

	TextWriter< StaticCtrl >::TextWriter( String const & tabs )
		: TextWriterT< StaticCtrl >{ tabs }
	{
	}

	bool TextWriter< StaticCtrl >::operator()( StaticCtrl const & control
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing StaticCtrl " ) << control.getName() << std::endl;
		return writeSub< Control >( file, control )
			&& writeName( file, cuT( "caption" ), control.getCaption() )
			&& writeOpt( file, cuT( "horizontal_align" ), getName( control.getHAlign() ), getName( HAlign::eLeft ) )
			&& writeOpt( file, cuT( "vertical_align" ), getName( control.getVAlign() ), getName( VAlign::eTop ) );
	}

	//*********************************************************************************************

	TextWriter< StaticStyle >::TextWriter( String const & tabs
		, String const & fontName )
		: TextWriterT< StaticStyle >{ tabs }
		, m_fontName{ fontName }
	{
	}

	bool TextWriter< StaticStyle >::operator()( StaticStyle const & style
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing StaticStyle " ) << style.getName() << std::endl;
		return writeSub< ControlStyle >( file, style )
			&& writeNameOpt( file, cuT( "font" ), style.getFontName(), m_fontName )
			&& ( style.getTextMaterial() ? writeName( file, cuT( "text_material" ), style.getTextMaterial()->getName() ) : true );
	}

	//*********************************************************************************************
}
