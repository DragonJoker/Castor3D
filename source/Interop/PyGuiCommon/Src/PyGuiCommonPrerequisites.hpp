/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GUIPY_PREREQUISITES_H___
#define ___GUIPY_PREREQUISITES_H___

#include <Render/RenderWindow.hpp>
#include <GuiCommonPrerequisites.hpp>

namespace cpy
{
	struct WindowHandleCreatorFromWindow
	{
		Castor3D::WindowHandle operator()( wxWindow * p_window )
		{
			return GuiCommon::make_WindowHandle( p_window );
		}
	};

	struct WindowHandleCreatorFromFrame
	{
		Castor3D::WindowHandle operator()( wxFrame * p_window )
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
			inline boost::mpl::vector< Castor3D::WindowHandle, wxWindow * >
			get_signature( cpy::WindowHandleCreatorFromWindow, void * = 0 )
			{
				return boost::mpl::vector< Castor3D::WindowHandle, wxWindow * >();
			}

			inline boost::mpl::vector< Castor3D::WindowHandle, wxFrame * >
			get_signature( cpy::WindowHandleCreatorFromFrame, void * = 0 )
			{
				return boost::mpl::vector< Castor3D::WindowHandle, wxFrame * >();
			}
		}
	}
}

#include <boost/python.hpp>
namespace py = boost::python;

#endif

