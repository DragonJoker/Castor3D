#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Event/UserInput/UserInputListener.hpp>
#include <Castor3D/Render/RenderWindow.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static const C3DString ERROR_UNINITIALISED_WINDOW = cuT( "The render window must be initialised" );
	static const C3DString ERROR_UNINITIALISED_WINTGT = cuT( "The render target must be initialised" );

	C3D_CAPIMETHODIMP c3dRenderWindow_delete( C3DRenderWindow * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			cc3d::reportWarning( ERROR_UNINITIALISED_WINDOW );

		object->internal.reset();
		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderWindow_getName( C3DRenderWindow const * object, C3DString * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_WINDOW );

		return cc3d::copyString( object->internal->getName(), result );
	}

	C3D_CAPIMETHODIMP c3dRenderWindow_initialise( C3DRenderWindow * object, C3DRenderTarget const * target )
	{
		if ( !object || !target )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_WINDOW );
		if ( !target->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_WINTGT );

		try
		{
			object->internal->initialise( c3d::RenderWindowDesc{ c3d::String{}
				, target->internal } );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderWindow_cleanup( C3DRenderWindow * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_WINDOW );

		try
		{
			object->internal->cleanup();
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderWindow_resize( C3DRenderWindow * object, C3DSize const * size )
	{
		if ( !object || !size )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_WINDOW );

		try
		{
			object->internal->resize( { size->width, size->height } );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderWindow_onMouseMove( C3DRenderWindow const * object, C3DPosition const * pos, bool isCtrlDown, bool isAltDown, bool isShiftDown, bool * result )
	{
		if ( !object || !pos )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_WINDOW );

		try
		{
			if ( auto inputListener = object->internal->getEngine()->getUserInputListener() )
			{
				*result = inputListener->fireMouseMove( c3d::Position{ pos->x, pos->y }, isCtrlDown, isAltDown, isShiftDown );
			}
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderWindow_onMouseLButtonDown( C3DRenderWindow const * object, bool isCtrlDown, bool isAltDown, bool isShiftDown, bool * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_WINDOW );

		try
		{
			if ( auto inputListener = object->internal->getEngine()->getUserInputListener() )
			{
				*result = inputListener->fireMouseButtonPushed( c3d::MouseButton::eLeft, isCtrlDown, isAltDown, isShiftDown );
			}
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderWindow_onMouseLButtonUp( C3DRenderWindow const * object, bool isCtrlDown, bool isAltDown, bool isShiftDown, bool * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_WINDOW );

		try
		{
			if ( auto inputListener = object->internal->getEngine()->getUserInputListener() )
			{
				*result = inputListener->fireMouseButtonReleased( c3d::MouseButton::eLeft, isCtrlDown, isAltDown, isShiftDown );
			}
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderWindow_onMouseMButtonDown( C3DRenderWindow const * object, bool isCtrlDown, bool isAltDown, bool isShiftDown, bool * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_WINDOW );

		try
		{
			if ( auto inputListener = object->internal->getEngine()->getUserInputListener() )
			{
				*result = inputListener->fireMouseButtonReleased( c3d::MouseButton::eMiddle, isCtrlDown, isAltDown, isShiftDown );
			}
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderWindow_onMouseMButtonUp( C3DRenderWindow const * object, bool isCtrlDown, bool isAltDown, bool isShiftDown, bool * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_WINDOW );

		try
		{
			if ( auto inputListener = object->internal->getEngine()->getUserInputListener() )
			{
				*result = inputListener->fireMouseButtonReleased( c3d::MouseButton::eMiddle, isCtrlDown, isAltDown, isShiftDown );
			}
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderWindow_onMouseRButtonDown( C3DRenderWindow const * object, bool isCtrlDown, bool isAltDown, bool isShiftDown, bool * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_WINDOW );

		try
		{
			if ( auto inputListener = object->internal->getEngine()->getUserInputListener() )
			{
				*result = inputListener->fireMouseButtonReleased( c3d::MouseButton::eRight, isCtrlDown, isAltDown, isShiftDown );
			}
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderWindow_onMouseRButtonUp( C3DRenderWindow const * object, bool isCtrlDown, bool isAltDown, bool isShiftDown, bool * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_WINDOW );

		try
		{
			if ( auto inputListener = object->internal->getEngine()->getUserInputListener() )
			{
				*result = inputListener->fireMouseButtonReleased( c3d::MouseButton::eRight, isCtrlDown, isAltDown, isShiftDown );
			}
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderWindow_onKeyboardKeyDown( C3DRenderWindow const * object, C3D_KEYBOARD_KEY key, bool isCtrlDown, bool isAltDown, bool isShiftDown, bool * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_WINDOW );

		try
		{
			if ( auto inputListener = object->internal->getEngine()->getUserInputListener() )
			{
				*result = inputListener->fireKeyDown( c3d::KeyboardKey( key ), isCtrlDown, isAltDown, isShiftDown );
			}
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderWindow_onKeyboardKeyUp( C3DRenderWindow const * object, C3D_KEYBOARD_KEY key, bool isCtrlDown, bool isAltDown, bool isShiftDown, bool * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_WINDOW );

		try
		{
			if ( auto inputListener = object->internal->getEngine()->getUserInputListener() )
			{
				*result = inputListener->fireKeyUp( c3d::KeyboardKey( key ), isCtrlDown, isAltDown, isShiftDown );
			}
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dRenderWindow_onKeyboardChar( C3DRenderWindow const * object, C3D_KEYBOARD_KEY key, C3DString c, bool * result )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_WINDOW );

		try
		{
			if ( auto inputListener = object->internal->getEngine()->getUserInputListener() )
			{
				*result = inputListener->fireChar( c3d::KeyboardKey( key ), c3d::String{ c } );
			}
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
