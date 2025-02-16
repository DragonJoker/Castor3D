//This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
//Copyright (c) 2016 dragonjoker59@hotmail.com

//Permission is hereby granted, free of charge, to any person obtaining a copy of
//this software and associated documentation files (the "Software"), to deal in
//the Software without restriction, including without limitation the rights to
//use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
//of the Software, and to permit persons to whom the Software is furnished to do
//so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.
#include "MainWindow.h"

#include <GLFW/glfw3.h>
#if defined( _WIN32 )
#	define GLFW_EXPOSE_NATIVE_WIN32
#elif defined( __linux__ )
#	define GLFW_EXPOSE_NATIVE_X11
#elif defined( __APPLE__ )
#	define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>
#include <string.h>

#include <CCastor3D/Castor3DPlatformTypes.h>

void doStartTimer( MainWindow * window, TIMER_ID id, TimerProc callback );
void doStopTimer( MainWindow * window, TIMER_ID id );

void onLeftTimer( MainWindow * window );
void onRightTimer( MainWindow * window );
void onForwardTimer( MainWindow * window );
void onBackTimer( MainWindow * window );
void onUpTimer( MainWindow * window );
void onDownTimer( MainWindow * window );
void onMotionTimer( MainWindow * window );

void onWindowResized( GLFWwindow * glfwWindow, int width, int height );
void onKeyboardKey( GLFWwindow * glfwWindow, int key, int scancode, int action, int mods );
void onMouseButton( GLFWwindow * glfwWindow, int button, int action, int mods );
void onMouseMove( GLFWwindow * glfwWindow, double xpos, double ypos );

bool windowCreate( GLFWwindow * glfwWindow, C3DEngine * engine, MainWindow * window )
{
	bool result = false;
	C3DWindowHandle handle;
	int ww = 0;
	int wh = 0;
	C3DSize size;

	glfwSetWindowUserPointer( glfwWindow, window );
	glfwSetWindowSizeCallback( glfwWindow, onWindowResized );
	glfwSetKeyCallback( glfwWindow, onKeyboardKey );
	glfwSetMouseButtonCallback( glfwWindow, onMouseButton );
	glfwSetCursorPosCallback( glfwWindow, onMouseMove );

	window->timers[IDT_MOTION].started = false;
	window->timers[IDT_LEFT].started = false;
	window->timers[IDT_RIGHT].started = false;
	window->timers[IDT_FORWARD].started = false;
	window->timers[IDT_BACK].started = false;
	window->timers[IDT_UP].started = false;
	window->timers[IDT_DOWN].started = false;

#if defined( _WIN32 )
	handle.hWnd = glfwGetWin32Window( glfwWindow );
#elif defined( __APPLE__ )
	handle.view = glfwGetCocoaWindow( glfwWindow );
#elif defined( __linux__ )
	handle.display = glfwGetX11Display();
	handle.drawable = glfwGetX11Window( glfwWindow );
#else
#	error "Unimplemented."
#endif

	glfwGetWindowSize( glfwWindow, &ww, &wh );
	size.width = ( uint32_t )ww;
	size.height = ( uint32_t )wh;
	if ( c3dRenderWindow_create( engine, "MainWindow", &size, handle, &window->renderWindow ) != C3D_OK )
		goto cleanup;

	if ( c3dRenderWindow_resize( window->renderWindow, &size ) != C3D_OK )
		goto cleanup;

	doStartTimer( window, IDT_MOTION, onMotionTimer );

end:
	return result;

cleanup:
	windowDestroy( window );
	goto end;
}

void windowDestroy( MainWindow * window )
{
	doStopTimer( window, IDT_MOTION );
	doStopTimer( window, IDT_LEFT );
	doStopTimer( window, IDT_RIGHT );
	doStopTimer( window, IDT_FORWARD );
	doStopTimer( window, IDT_BACK );
	doStopTimer( window, IDT_UP );
	doStopTimer( window, IDT_DOWN );

	if ( window->renderWindow )
	{
		c3dRenderWindow_delete( window->renderWindow );
		window->renderWindow = NULL;
	}
}

void doStartTimer( MainWindow * window, TIMER_ID id, TimerProc callback )
{
	if ( window )
	{
		window->timers[id].startTime = clock();
		window->timers[id].callback = callback;
		window->timers[id].started = true;
	}
}

void doStopTimer( MainWindow * window, TIMER_ID id )
{
	if ( window )
	{
		window->timers[id].started = false;
	}
}

void onLeftTimer( MainWindow * window )
{
	if ( window )
	{
		float speed = 1.0f;
		stateAddScalarVelocity( &window->nodeState, speed, 0.0f, 0.0f );
	}
}

void onRightTimer( MainWindow * window )
{
	if ( window )
	{
		float speed = 1.0f;
		stateAddScalarVelocity( &window->nodeState, -speed, 0.0f, 0.0f );
	}
}

void onForwardTimer( MainWindow * window )
{
	if ( window )
	{
		float speed = 1.0f;
		stateAddScalarVelocity( &window->nodeState, 0.0f, 0.0f, speed );
	}
}

void onBackTimer( MainWindow * window )
{
	if ( window )
	{
		float speed = 1.0f;
		stateAddScalarVelocity( &window->nodeState, 0.0f, 0.0f, -speed );
	}
}

void onUpTimer( MainWindow * window )
{
	if ( window )
	{
		float speed = 1.0f;
		stateAddScalarVelocity( &window->nodeState, 0.0f, speed, 0.0f );
	}
}

void onDownTimer( MainWindow * window )
{
	if ( window )
	{
		float speed = 1.0f;
		stateAddScalarVelocity( &window->nodeState, 0.0f, -speed, 0.0f );
	}
}

void onMotionTimer( MainWindow * window )
{
	if ( window )
	{
		stateUpdate( &window->nodeState );
	}
}

static C3DVec2 doTransform( const MainWindow * window, double x, double y, double ww, double wh )
{
	uint32_t cw = 0;
	uint32_t ch = 0;
	C3DVec2 result;
	c3dCamera_getWidth( window->camera, &cw );
	c3dCamera_getHeight( window->camera, &ch );
	result.x = ( float )( ( x * cw ) / ww );
	result.y = ( float )( ( y * ch ) / wh );
	return result;
}

static C3D_KEYBOARD_KEY doConvertScanCode( int scancode )
{
	switch ( scancode )
	{
	case GLFW_KEY_BACKSPACE:
		return C3D_KEYBOARD_KEY_BACKSPACE;
	case GLFW_KEY_TAB:
		return C3D_KEYBOARD_KEY_TAB;
	case GLFW_KEY_ENTER:
		return C3D_KEYBOARD_KEY_RETURN;
	case GLFW_KEY_LEFT_SHIFT:
	case GLFW_KEY_RIGHT_SHIFT:
		return C3D_KEYBOARD_KEY_SHIFT;
	case GLFW_KEY_LEFT_CONTROL:
	case GLFW_KEY_RIGHT_CONTROL:
		return C3D_KEYBOARD_KEY_CONTROL;
	case GLFW_KEY_MENU:
		return C3D_KEYBOARD_KEY_MENU;
	case GLFW_KEY_PAUSE:
		return C3D_KEYBOARD_KEY_PAUSE;
	case GLFW_KEY_CAPS_LOCK:
		return C3D_KEYBOARD_KEY_CAPITAL;
	case GLFW_KEY_ESCAPE:
		return C3D_KEYBOARD_KEY_ESCAPE;
	case GLFW_KEY_SPACE:
		return C3D_KEYBOARD_KEY_SPACE;
	case GLFW_KEY_END:
		return C3D_KEYBOARD_KEY_END;
	case GLFW_KEY_HOME:
		return C3D_KEYBOARD_KEY_HOME;
	case GLFW_KEY_LEFT:
		return C3D_KEYBOARD_KEY_LEFT;
	case GLFW_KEY_UP:
		return C3D_KEYBOARD_KEY_UP;
	case GLFW_KEY_RIGHT:
		return C3D_KEYBOARD_KEY_RIGHT;
	case GLFW_KEY_DOWN:
		return C3D_KEYBOARD_KEY_DOWN;
	case GLFW_KEY_PAGE_UP:
		return C3D_KEYBOARD_KEY_PAGEUP;
	case GLFW_KEY_PAGE_DOWN:
		return C3D_KEYBOARD_KEY_PAGEDOWN;
	case GLFW_KEY_PRINT_SCREEN:
		return C3D_KEYBOARD_KEY_SNAPSHOT;
	case GLFW_KEY_INSERT:
		return C3D_KEYBOARD_KEY_INSERT;
	case GLFW_KEY_DELETE:
		return C3D_KEYBOARD_KEY_DELETE;
	case GLFW_KEY_KP_0:
		return C3D_KEYBOARD_KEY_NUMPAD0;
	case GLFW_KEY_KP_1:
		return C3D_KEYBOARD_KEY_NUMPAD1;
	case GLFW_KEY_KP_2:
		return C3D_KEYBOARD_KEY_NUMPAD2;
	case GLFW_KEY_KP_3:
		return C3D_KEYBOARD_KEY_NUMPAD3;
	case GLFW_KEY_KP_4:
		return C3D_KEYBOARD_KEY_NUMPAD4;
	case GLFW_KEY_KP_5:
		return C3D_KEYBOARD_KEY_NUMPAD5;
	case GLFW_KEY_KP_6:
		return C3D_KEYBOARD_KEY_NUMPAD6;
	case GLFW_KEY_KP_7:
		return C3D_KEYBOARD_KEY_NUMPAD7;
	case GLFW_KEY_KP_8:
		return C3D_KEYBOARD_KEY_NUMPAD8;
	case GLFW_KEY_KP_9:
		return C3D_KEYBOARD_KEY_NUMPAD9;
	case GLFW_KEY_KP_MULTIPLY:
		return C3D_KEYBOARD_KEY_MULTIPLY;
	case GLFW_KEY_KP_ADD:
		return C3D_KEYBOARD_KEY_ADD;
	case GLFW_KEY_KP_SUBTRACT:
		return C3D_KEYBOARD_KEY_SUBTRACT;
	case GLFW_KEY_KP_DECIMAL:
		return C3D_KEYBOARD_KEY_DECIMAL;
	case GLFW_KEY_KP_DIVIDE:
		return C3D_KEYBOARD_KEY_DIVIDE;
	case GLFW_KEY_F1:
		return C3D_KEYBOARD_KEY_F1;
	case GLFW_KEY_F2:
		return C3D_KEYBOARD_KEY_F2;
	case GLFW_KEY_F3:
		return C3D_KEYBOARD_KEY_F3;
	case GLFW_KEY_F4:
		return C3D_KEYBOARD_KEY_F4;
	case GLFW_KEY_F5:
		return C3D_KEYBOARD_KEY_F5;
	case GLFW_KEY_F6:
		return C3D_KEYBOARD_KEY_F6;
	case GLFW_KEY_F7:
		return C3D_KEYBOARD_KEY_F7;
	case GLFW_KEY_F8:
		return C3D_KEYBOARD_KEY_F8;
	case GLFW_KEY_F9:
		return C3D_KEYBOARD_KEY_F9;
	case GLFW_KEY_F10:
		return C3D_KEYBOARD_KEY_F10;
	case GLFW_KEY_F11:
		return C3D_KEYBOARD_KEY_F11;
	case GLFW_KEY_F12:
		return C3D_KEYBOARD_KEY_F12;
	case GLFW_KEY_F13:
		return C3D_KEYBOARD_KEY_F13;
	case GLFW_KEY_F14:
		return C3D_KEYBOARD_KEY_F14;
	case GLFW_KEY_F15:
		return C3D_KEYBOARD_KEY_F15;
	case GLFW_KEY_F16:
		return C3D_KEYBOARD_KEY_F16;
	case GLFW_KEY_F17:
		return C3D_KEYBOARD_KEY_F17;
	case GLFW_KEY_F18:
		return C3D_KEYBOARD_KEY_F18;
	case GLFW_KEY_F19:
		return C3D_KEYBOARD_KEY_F19;
	case GLFW_KEY_F20:
		return C3D_KEYBOARD_KEY_F20;
	case GLFW_KEY_F21:
		return C3D_KEYBOARD_KEY_F21;
	case GLFW_KEY_F22:
		return C3D_KEYBOARD_KEY_F22;
	case GLFW_KEY_F23:
		return C3D_KEYBOARD_KEY_F23;
	case GLFW_KEY_F24:
		return C3D_KEYBOARD_KEY_F24;
	case GLFW_KEY_NUM_LOCK:
		return C3D_KEYBOARD_KEY_NUMLOCK;
	case GLFW_KEY_SCROLL_LOCK:
		return C3D_KEYBOARD_KEY_SCROLL;
	default:
		return ( C3D_KEYBOARD_KEY )scancode;
	}
}

void onWindowResized( GLFWwindow * glfwWindow, int width, int height )
{
	MainWindow * window = ( MainWindow * )glfwGetWindowUserPointer( glfwWindow );
	C3DSize size;

	if ( window && window->renderTarget )
	{
		size.width = ( uint32_t )width;
		size.height = ( uint32_t )height;
		c3dRenderWindow_resize( window->renderWindow, &size );
	}
}

void onKeyboardKey( GLFWwindow * glfwWindow, int glfwKey, int scancode, int action, int mods )
{
	MainWindow * window = ( MainWindow * )glfwGetWindowUserPointer( glfwWindow );
	bool processed;
	C3D_KEYBOARD_KEY key;

	if ( window && window->renderTarget )
	{
		key = doConvertScanCode( glfwKey );
		window->isShiftDown = ( mods & GLFW_MOD_SHIFT ) != 0;
		window->isCtrlDown = ( mods & GLFW_MOD_CONTROL ) != 0;
		window->isAltDown = ( mods & GLFW_MOD_ALT ) != 0;

		if ( action == GLFW_PRESS )
		{
			c3dRenderWindow_onKeyboardKeyDown( window->renderWindow, key, window->isCtrlDown, window->isAltDown, window->isShiftDown, &processed );
			if ( !processed )
			{
				switch ( glfwKey )
				{
				case GLFW_KEY_LEFT:
				case GLFW_KEY_A:
					doStartTimer( window, IDT_LEFT, onLeftTimer );
					break;
				case GLFW_KEY_RIGHT:
				case GLFW_KEY_D:
					doStartTimer( window, IDT_RIGHT, onRightTimer );
					break;
				case GLFW_KEY_UP:
				case GLFW_KEY_W:
					doStartTimer( window, IDT_FORWARD, onForwardTimer );
					break;
				case GLFW_KEY_DOWN:
				case GLFW_KEY_S:
					doStartTimer( window, IDT_BACK, onBackTimer );
					break;
				case GLFW_KEY_PAGE_UP:
				case GLFW_KEY_E:
					doStartTimer( window, IDT_UP, onUpTimer );
					break;
				case GLFW_KEY_PAGE_DOWN:
				case GLFW_KEY_Q:
					doStartTimer( window, IDT_DOWN, onDownTimer );
					break;
				default:
					break;
				}
			}
		}
		else if ( action == GLFW_RELEASE )
		{
			c3dRenderWindow_onKeyboardKeyUp( window->renderWindow, key, window->isCtrlDown, window->isAltDown, window->isShiftDown, &processed );
			if ( !processed )
			{
				switch ( glfwKey )
				{
				case GLFW_KEY_LEFT:
				case GLFW_KEY_A:
					doStopTimer( window, IDT_LEFT );
					break;
				case GLFW_KEY_RIGHT:
				case GLFW_KEY_D:
					doStopTimer( window, IDT_RIGHT );
					break;
				case GLFW_KEY_UP:
				case GLFW_KEY_W:
					doStopTimer( window, IDT_FORWARD );
					break;
				case GLFW_KEY_DOWN:
				case GLFW_KEY_S:
					doStopTimer( window, IDT_BACK );
					break;
				case GLFW_KEY_PAGE_UP:
				case GLFW_KEY_E:
					doStopTimer( window, IDT_UP );
					break;
				case GLFW_KEY_PAGE_DOWN:
				case GLFW_KEY_Q:
					doStopTimer( window, IDT_DOWN );
					break;
				default:
					break;
				}
			}
		}
	}
}

void onMouseButton( GLFWwindow * glfwWindow, int button, int action, int mods )
{
	MainWindow * window = ( MainWindow * )glfwGetWindowUserPointer( glfwWindow );
	bool processed;
	double xpos = 0;
	double ypos = 0;
	int ww = 0;
	int wh = 0;

	if ( window && window->renderTarget )
	{
		window->isShiftDown = ( mods & GLFW_MOD_SHIFT ) != 0;
		window->isCtrlDown = ( mods & GLFW_MOD_CONTROL ) != 0;
		window->isAltDown = ( mods & GLFW_MOD_ALT ) != 0;

		if ( action == GLFW_PRESS )
		{
			if ( button == GLFW_MOUSE_BUTTON_LEFT )
			{
				c3dRenderWindow_onMouseLButtonDown( window->renderWindow, window->isCtrlDown, window->isAltDown, window->isShiftDown, &processed );
			}
			else if ( button == GLFW_MOUSE_BUTTON_MIDDLE )
			{
				c3dRenderWindow_onMouseMButtonDown( window->renderWindow, window->isCtrlDown, window->isAltDown, window->isShiftDown, &processed );
			}
			else if ( button == GLFW_MOUSE_BUTTON_RIGHT )
			{
				c3dRenderWindow_onMouseRButtonDown( window->renderWindow, window->isCtrlDown, window->isAltDown, window->isShiftDown, &processed );
			}
		}
		else if ( action == GLFW_RELEASE )
		{
			if ( button == GLFW_MOUSE_BUTTON_LEFT )
			{
				c3dRenderWindow_onMouseLButtonUp( window->renderWindow, window->isCtrlDown, window->isAltDown, window->isShiftDown, &processed );
			}
			else if ( button == GLFW_MOUSE_BUTTON_MIDDLE )
			{
				c3dRenderWindow_onMouseMButtonUp( window->renderWindow, window->isCtrlDown, window->isAltDown, window->isShiftDown, &processed );
			}
			else if ( button == GLFW_MOUSE_BUTTON_RIGHT )
			{
				c3dRenderWindow_onMouseRButtonUp( window->renderWindow, window->isCtrlDown, window->isAltDown, window->isShiftDown, &processed );
			}
		}

		glfwGetCursorPos( glfwWindow, &xpos, &ypos );
		glfwGetWindowSize( glfwWindow, &ww, &wh );
		window->oldPosition = doTransform( window, xpos, ypos, ww, wh );
	}
}

void onMouseMove( GLFWwindow * glfwWindow, double xpos, double ypos )
{
	MainWindow * window = ( MainWindow * )glfwGetWindowUserPointer( glfwWindow );
	int ww = 0;
	int wh = 0;
	bool processed;
	float mult = 8.0f;
	C3DVec2 newPosition;
	C3DPosition pos;
	float deltaX;
	float deltaY;
	bool leftDown;
	bool rightDown;

	if ( window && window->renderTarget )
	{
		glfwGetWindowSize( glfwWindow, &ww, &wh );
		newPosition = doTransform( window, xpos, ypos, ww, wh );
		pos.x = ( int )newPosition.x;
		pos.y = ( int )newPosition.y;
		c3dRenderWindow_onMouseMove( window->renderWindow, &pos, window->isCtrlDown, window->isAltDown, window->isShiftDown, &processed );
		if ( !processed )
		{
			deltaX = ( window->oldPosition.x - newPosition.x ) / mult;
			deltaY = ( window->oldPosition.y - newPosition.y ) / mult;
			leftDown = ( GLFW_PRESS == glfwGetMouseButton( glfwWindow, GLFW_MOUSE_BUTTON_LEFT ) );
			rightDown = ( GLFW_PRESS == glfwGetMouseButton( glfwWindow, GLFW_MOUSE_BUTTON_RIGHT ) );

			if ( leftDown )
			{
				stateAddAngularVelocity( &window->nodeState, -deltaY, deltaX );
			}
			else if ( rightDown )
			{
				stateAddScalarVelocity( &window->nodeState, deltaX, -deltaY, 0.0f );
			}
		}

		window->oldPosition = newPosition;
	}
}
