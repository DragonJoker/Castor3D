/* See LICENSE file in root folder */
#ifndef ___GUIPY_PREREQUISITES_H___
#define ___GUIPY_PREREQUISITES_H___

#include <Render/RenderWindow.hpp>
#include <GuiCommonPrerequisites.hpp>

namespace cpy
{
	struct WindowHandleCreatorFromWindow
	{
		renderer::WindowHandle operator()( wxWindow * p_window )
		{
			return GuiCommon::makeWindowHandle( p_window );
		}
	};

	struct WindowHandleCreatorFromFrame
	{
		renderer::WindowHandle operator()( wxFrame * p_window )
		{
			return GuiCommon::makeWindowHandle( p_window );
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
			inline boost::mpl::vector< renderer::WindowHandle, wxWindow * >
			get_signature( cpy::WindowHandleCreatorFromWindow, void * = 0 )
			{
				return boost::mpl::vector< renderer::WindowHandle, wxWindow * >();
			}

			inline boost::mpl::vector< renderer::WindowHandle, wxFrame * >
			get_signature( cpy::WindowHandleCreatorFromFrame, void * = 0 )
			{
				return boost::mpl::vector< renderer::WindowHandle, wxFrame * >();
			}
		}
	}
}

#include <boost/python.hpp>
namespace py = boost::python;

#endif

