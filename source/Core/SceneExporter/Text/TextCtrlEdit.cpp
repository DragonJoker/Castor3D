#include "TextCtrlEdit.hpp"

#include "TextCtrlControl.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>

namespace castor
{
	using namespace castor3d;

	//*********************************************************************************************

	TextWriter< EditCtrl >::TextWriter( String const & tabs )
		: TextWriterT< EditCtrl >{ tabs }
	{
	}

	bool TextWriter< EditCtrl >::operator()( EditCtrl const & control
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing EditCtrl " ) << control.getName() << std::endl;
		return writeSub< Control >( file, control )
			&& writeOpt( file, "multiline", control.isMultiLine(), false )
			&& writeName( file, "caption", control.getCaption() );
	}

	//*********************************************************************************************

	TextWriter< EditStyle >::TextWriter( String const & tabs
		, String const & fontName )
		: TextWriterT< EditStyle >{ tabs }
		, m_fontName{ fontName }
	{
	}

	bool TextWriter< EditStyle >::operator()( EditStyle const & style
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing EditStyle " ) << style.getName() << std::endl;
		return writeSub< ControlStyle >( file, style )
			&& writeNameOpt( file, cuT( "font" ), style.getFontName(), m_fontName )
			&& ( style.getTextMaterial() ? writeName( file, cuT( "text_material" ), style.getTextMaterial()->getName() ) : true );
	}

	//*********************************************************************************************
}
