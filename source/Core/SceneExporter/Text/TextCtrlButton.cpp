#include "TextCtrlButton.hpp"

#include "TextCtrlControl.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>

namespace castor
{
	using namespace castor3d;

	//*********************************************************************************************

	namespace btnwrtr
	{
		static bool isWritableMaterial( MaterialObs material )
		{
			if ( !material )
			{
				return false;
			}

			auto name = material->getName();
			return !string::endsWith( name, cuT( "_THG" ) )
				&& !string::endsWith( name, cuT( "_BHG" ) )
				&& !string::endsWith( name, cuT( "_FHG" ) )
				&& !string::endsWith( name, cuT( "_BPU" ) )
				&& !string::endsWith( name, cuT( "_FPU" ) )
				&& !string::endsWith( name, cuT( "_TDS" ) )
				&& !string::endsWith( name, cuT( "_BDS" ) )
				&& !string::endsWith( name, cuT( "_FDS" ) );
		}
	}

	//*********************************************************************************************

	TextWriter< ButtonCtrl >::TextWriter( String const & tabs )
		: TextWriterT< ButtonCtrl >{ tabs }
	{
	}

	bool TextWriter< ButtonCtrl >::operator()( ButtonCtrl const & control
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing ButtonCtrl " ) << control.getName() << std::endl;
		return writeSub< Control >( file, control )
			&& writeName( file, cuT( "caption" ), makeString( control.getCaption() ) )
			&& writeOpt( file, cuT( "horizontal_align" ), getName( control.getHAlign() ), getName( HAlign::eLeft ) )
			&& writeOpt( file, cuT( "vertical_align" ), getName( control.getVAlign() ), getName( VAlign::eTop ) );
	}

	//*********************************************************************************************

	TextWriter< ButtonStyle >::TextWriter( String const & tabs
		, String const & fontName )
		: TextWriterT< ButtonStyle >{ tabs }
		, m_fontName{ fontName }
	{
	}

	bool TextWriter< ButtonStyle >::operator()( ButtonStyle const & style
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing ButtonStyle " ) << style.getName() << std::endl;
		return writeSub< ControlStyle >( file, style )
			&& writeNameOpt( file, cuT( "font" ), style.getFontName(), m_fontName )
			&& ( style.getTextMaterial() ? writeName( file, cuT( "text_material" ), style.getTextMaterial()->getName() ) : true )
			&& ( btnwrtr::isWritableMaterial( style.getHighlightedBackgroundMaterial()  ) ? writeName( file, cuT( "highlighted_background_material" ), style.getHighlightedBackgroundMaterial()->getName() ) : true )
			&& ( btnwrtr::isWritableMaterial( style.getHighlightedForegroundMaterial()  ) ? writeName( file, cuT( "highlighted_foreground_material" ), style.getHighlightedForegroundMaterial()->getName() ) : true )
			&& ( btnwrtr::isWritableMaterial( style.getHighlightedTextMaterial()  ) ? writeName( file, cuT( "highlighted_text_material" ), style.getHighlightedTextMaterial()->getName() ) : true )
			&& ( btnwrtr::isWritableMaterial( style.getPushedBackgroundMaterial()  ) ? writeName( file, cuT( "pushed_background_material" ), style.getPushedBackgroundMaterial()->getName() ) : true )
			&& ( btnwrtr::isWritableMaterial( style.getPushedForegroundMaterial()  ) ? writeName( file, cuT( "pushed_foreground_material" ), style.getPushedForegroundMaterial()->getName() ) : true )
			&& ( btnwrtr::isWritableMaterial( style.getPushedTextMaterial()  ) ? writeName( file, cuT( "pushed_text_material" ), style.getPushedTextMaterial()->getName() ) : true )
			&& ( btnwrtr::isWritableMaterial( style.getDisabledBackgroundMaterial()  ) ? writeName( file, cuT( "disabled_background_material" ), style.getDisabledBackgroundMaterial()->getName() ) : true )
			&& ( btnwrtr::isWritableMaterial( style.getDisabledForegroundMaterial()  ) ? writeName( file, cuT( "disabled_foreground_material" ), style.getDisabledForegroundMaterial()->getName() ) : true )
			&& ( btnwrtr::isWritableMaterial( style.getDisabledTextMaterial()  ) ? writeName( file, cuT( "disabled_text_material" ), style.getDisabledTextMaterial()->getName() ) : true );
	}

	//*********************************************************************************************
}
