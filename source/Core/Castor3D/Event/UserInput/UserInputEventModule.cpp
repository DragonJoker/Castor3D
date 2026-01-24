#include "Castor3D/Event/UserInput/UserInputEventModule.hpp"

#include "Castor3D/Event/UserInput/EventHandler.hpp"

CU_ImplementSmartPtr( c3d, EventHandler )

namespace c3d
{
	String getName( UserInputEventType value )
	{
		switch ( value )
		{
		case UserInputEventType::eMouse:
			return cuT( "mouse" );
		case UserInputEventType::eKeyboard:
			return cuT( "keyboard" );
		case UserInputEventType::eHandler:
			return cuT( "handler" );
		default:
			CU_Failure( "Unsupported UserInputEventType" );
			return cuEmptyString;
		}
	}

	String getName( MouseEventType value )
	{
		switch ( value )
		{
		case MouseEventType::eMove:
			return cuT( "move" );
		case MouseEventType::eEnter:
			return cuT( "enter" );
		case MouseEventType::eHover:
			return cuT( "hover" );
		case MouseEventType::eLeave:
			return cuT( "leaver" );
		case MouseEventType::ePushed:
			return cuT( "pushed" );
		case MouseEventType::eReleased:
			return cuT( "released" );
		case MouseEventType::eWheel:
			return cuT( "wheel" );
		default:
			CU_Failure( "Unsupported MouseEventType" );
			return cuEmptyString;
		}
	}

	String getName( KeyboardEventType value )
	{
		switch ( value )
		{
		case KeyboardEventType::ePushed:
			return cuT( "pushed" );
		case KeyboardEventType::eReleased:
			return cuT( "released" );
		case KeyboardEventType::eChar:
			return cuT( "char" );
		default:
			CU_Failure( "Unsupported KeyboardEventType" );
			return cuEmptyString;
		}
	}

	String getName( HandlerEventType value )
	{
		switch ( value )
		{
		case HandlerEventType::eActivate:
			return cuT( "activate" );
		case HandlerEventType::eDeactivate:
			return cuT( "deactivate" );
		default:
			CU_Failure( "Unsupported HandlerEventType" );
			return cuEmptyString;
		}
	}

	String getName( MouseButton value )
	{
		switch ( value )
		{
		case MouseButton::eLeft:
			return cuT( "left" );
		case MouseButton::eMiddle:
			return cuT( "middle" );
		case MouseButton::eRight:
			return cuT( "right" );
		default:
			CU_Failure( "Unsupported MouseButton" );
			return cuEmptyString;
		}
	}
}
