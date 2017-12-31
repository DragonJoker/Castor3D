/* See LICENSE file in root folder */
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

