#include "Castor3D/Overlay/OverlayModule.hpp"

namespace c3d
{
	String getName( OverlayType value )
	{
		switch ( value )
		{
		case OverlayType::ePanel:
			return cuT( "panel" );
		case OverlayType::eBorderPanel:
			return cuT( "border_panel" );
		case OverlayType::eText:
			return cuT( "text" );
		default:
			CU_Failure( "Unsupported OverlayType" );
			return cuEmptyString;
		}
	}

	String getName( TextWrappingMode value )
	{
		switch ( value )
		{
		case TextWrappingMode::eNone:
			return cuT( "none" );
		case TextWrappingMode::eBreak:
			return cuT( "break" );
		case TextWrappingMode::eBreakWords:
			return cuT( "break_words" );
		default:
			CU_Failure( "Unsupported TextWrappingMode" );
			return cuEmptyString;
		}
	}

	String getName( TextTexturingMode value )
	{
		switch ( value )
		{
		case TextTexturingMode::eLetter:
			return cuT( "letter" );
		case TextTexturingMode::eText:
			return cuT( "text" );
		default:
			CU_Failure( "Unsupported TextTexturingMode" );
			return cuEmptyString;
		}
	}

	String getName( TextLineSpacingMode value )
	{
		switch ( value )
		{
		case TextLineSpacingMode::eOwnHeight:
			return cuT( "own_height" );
		case TextLineSpacingMode::eMaxLineHeight:
			return cuT( "max_lines_height" );
		case TextLineSpacingMode::eMaxFontHeight:
			return cuT( "max_font_height" );
		default:
			CU_Failure( "Unsupported TextLineSpacingMode" );
			return cuEmptyString;
		}
	}

	String getName( HAlign value )
	{
		switch ( value )
		{
		case HAlign::eLeft:
			return cuT( "left" );
		case HAlign::eCenter:
			return cuT( "center" );
		case HAlign::eRight:
			return cuT( "right" );
		default:
			CU_Failure( "Unsupported HAlign" );
			return cuEmptyString;
		}
	}

	String getName( VAlign value )
	{
		switch ( value )
		{
		case VAlign::eTop:
			return cuT( "top" );
		case VAlign::eCenter:
			return cuT( "center" );
		case VAlign::eBottom:
			return cuT( "bottom" );
		default:
			CU_Failure( "Unsupported VAlign" );
			return cuEmptyString;
		}
	}

	String getName( BorderPosition value )
	{
		switch ( value )
		{
		case BorderPosition::eInternal:
			return cuT( "internal" );
		case BorderPosition::eMiddle:
			return cuT( "middle" );
		case BorderPosition::eExternal:
			return cuT( "external" );
		default:
			CU_Failure( "Unsupported BorderPosition" );
			return cuEmptyString;
		}
	}
}
