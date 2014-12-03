#include "GuiCommonPch.hpp"

#if defined( _MSC_VER ) && !defined( NDEBUG ) && !defined( VLD_AVAILABLE )
#	define _CRTDBG_MAP_ALLOC
#	include <cstdlib>
#	include <crtdbg.h>
#elif defined( __linux__ )
#	include <gdk/gdkx.h>
#	include <gtk/gtk.h>
#endif

#include <PlatformWindowHandle.hpp>

#include <wx/window.h>

namespace GuiCommon
{
	Castor3D::WindowHandle wxMakeWindowHandle( wxWindow * p_window )
	{
#if defined( _WIN32 )
		return Castor3D::WindowHandle( std::make_shared< Castor3D::IMswWindowHandle >( p_window->GetHandle() ) );
#elif defined( __linux__ )
		GtkWidget * l_pGtkWidget = static_cast< GtkWidget * >( p_window->GetHandle() );
		GLXDrawable l_drawable = None;
		Display * l_pDisplay = NULL;

		if ( l_pGtkWidget && l_pGtkWidget->window )
		{
			l_drawable = GDK_WINDOW_XID( l_pGtkWidget->window );
			GdkDisplay * l_pGtkDisplay = gtk_widget_get_display( l_pGtkWidget );

			if ( l_pGtkDisplay )
			{
				l_pDisplay = gdk_x11_display_get_xdisplay( l_pGtkDisplay );
			}
		}

		return Castor3D::WindowHandle( std::make_shared< Castor3D::IXWindowHandle >( l_drawable, l_pDisplay ) );
#else
#	error "Yet unsupported OS"
#endif
	}
}
