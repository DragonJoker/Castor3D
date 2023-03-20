#include "TextCtrlControl.hpp"

#include "TextCtrlControl.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>
#include <CastorUtils/Data/Text/TextPosition.hpp>
#include <CastorUtils/Data/Text/TextSize.hpp>

namespace castor
{
	using namespace castor3d;

	//*********************************************************************************************

	TextWriter< Control >::TextWriter( String const & tabs
		, bool customStyle )
		: TextWriterT< Control >{ tabs }
		, m_customStyle{ customStyle }
	{
	}

	bool TextWriter< Control >::operator()( Control const & control
		, StringStream & file )
	{
		bool result = true;

		if ( !m_customStyle )
		{
			result = writeName( file, cuT( "style" ), control.getBaseStyle().getName() )
				&& writeOpt( file, cuT( "visible" ), control.isVisible(), true )
				&& writeOpt( file, cuT( "movable" ), control.isMovable(), false )
				&& writeOpt( file, cuT( "resizable" ), control.isResizable(), false )
				&& writeNamedSubOpt( file, cuT( "pixel_position" ), control.getPosition(), Position{} )
				&& writeNamedSubOpt( file, cuT( "pixel_size" ), control.getSize(), Size{} );
		}

		return result
			&& writeNamedSubOpt( file, cuT( "pixel_border_size" ), control.getBorderSize(), Point4ui{} )
			&& writeNamedSubOpt( file, cuT( "border_inner_uv" ), control.getBorderInnerUV(), Point4d{ 0.33, 0.33, 0.66, 0.66 } )
			&& writeNamedSubOpt( file, cuT( "border_outer_uv" ), control.getBorderOuterUV(), Point4d{ 0, 0, 1, 1 } )
			&& writeNamedSubOpt( file, cuT( "center_uv" ), control.getUV(), Point4d{ 0.0, 0.0, 1.0, 1.0 } );
	}

	//*********************************************************************************************

	TextWriter< ControlStyle >::TextWriter( String const & tabs )
		: TextWriterT< ControlStyle >{ tabs }
	{
	}

	bool TextWriter< ControlStyle >::operator()( ControlStyle const & style
		, StringStream & file )
	{
		return writeOpt( file, cuT( "background_invisible" ), style.isBackgroundInvisible(), false )
			&& writeOpt( file, cuT( "foreground_invisible" ), style.isForegroundInvisible(), false )
			&& ( style.getBackgroundMaterial()
				? writeName( file, cuT( "background_material" ), style.getBackgroundMaterial()->getName() )
				: write( file, cuT( "background_invisible" ), true ) )
			&& ( style.getForegroundMaterial()
				? writeName( file, cuT( "foreground_material" ), style.getForegroundMaterial()->getName() )
				: write( file, cuT( "foreground_invisible" ), true ) );
	}

	//*********************************************************************************************
}
