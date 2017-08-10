/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___GUIPY_PREREQUISITES_H___
#define ___GUIPY_PREREQUISITES_H___

#include <Render/RenderWindow.hpp>
#include <GuiCommonPrerequisites.hpp>

namespace cpy
{
	struct WindowHandleCreatorFromWindow
	{
		castor3d::WindowHandle operator()( wxWindow * p_window )
		{
			return GuiCommon::make_WindowHandle( p_window );
		}
	};

	struct WindowHandleCreatorFromFrame
	{
		castor3d::WindowHandle operator()( wxFrame * p_window )
		{
			return GuiCommon::make_WindowHandle( p_window );
		}
	};
}

#include <boost/mpl/vector.hpp>

namespace boost
{
	namespace python
	{
		namespace detail
		{
			inline boost::mpl::vector< castor3d::WindowHandle, wxWindow * >
			get_signature( cpy::WindowHandleCreatorFromWindow, void * = 0 )
			{
				return boost::mpl::vector< castor3d::WindowHandle, wxWindow * >();
			}

			inline boost::mpl::vector< castor3d::WindowHandle, wxFrame * >
			get_signature( cpy::WindowHandleCreatorFromFrame, void * = 0 )
			{
				return boost::mpl::vector< castor3d::WindowHandle, wxFrame * >();
			}
		}
	}
}

#include <boost/python.hpp>
namespace py = boost::python;

#endif

