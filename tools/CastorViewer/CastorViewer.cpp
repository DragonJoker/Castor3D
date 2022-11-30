#include "CastorViewer/CastorViewer.hpp"
#include "CastorViewer/MainFrame.hpp"

#include <wx/cmdline.h>

#include <GuiCommon/System/ImagesLoader.hpp>
#include <GuiCommon/System/RendererSelector.hpp>

#include <GuiCommon/xpms/export.xpm>
#include <GuiCommon/xpms/log.xpm>
#include <GuiCommon/xpms/mat_blanc.xpm>
#include <GuiCommon/xpms/print_screen.xpm>
#include <GuiCommon/xpms/properties.xpm>
#include <GuiCommon/xpms/scene_blanc.xpm>

#include <CastorUtils/Data/File.hpp>
#include <CastorUtils/Exception/Exception.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
wxIMPLEMENT_APP( CastorViewer::CastorViewerApp );
#pragma GCC diagnostic pop

namespace CastorViewer
{
	namespace
	{
		static const bool isCastor3DThreaded = true;
#if defined( NDEBUG )
		static const int wantedFPS = 1000;
#else
		static const int wantedFPS = 60;
#endif

		castor3d::Version getVersion()
		{
			return castor3d::Version
			{
				CastorViewer_VERSION_MAJOR,
				CastorViewer_VERSION_MINOR,
				CastorViewer_VERSION_BUILD
			};
		}
	}

	CastorViewerApp::CastorViewerApp()
		: CastorApplication{ cuT( "CastorViewer" )
			, cuT( "Castor Viewer" )
			, 7
			, CastorViewer::getVersion()
			, wantedFPS
			, isCastor3DThreaded }
		, m_mainFrame( nullptr )
	{
		wxSetAssertHandler( &CastorApplication::assertHandler );
	}

	void CastorViewerApp::doLoadAppImages()
	{
		GuiCommon::ImagesLoader::addBitmap( eBMP_SCENES, scene_blanc_xpm );
		GuiCommon::ImagesLoader::addBitmap( eBMP_MATERIALS, mat_blanc_xpm );
		GuiCommon::ImagesLoader::addBitmap( eBMP_EXPORT, export_xpm );
		GuiCommon::ImagesLoader::addBitmap( eBMP_LOGS, log_xpm );
		GuiCommon::ImagesLoader::addBitmap( eBMP_PROPERTIES, properties_xpm );
		GuiCommon::ImagesLoader::addBitmap( eBMP_PRINTSCREEN, print_screen_xpm );
	}

	wxWindow * CastorViewerApp::doInitialiseMainFrame( GuiCommon::SplashScreen & splashScreen )
	{
		wxAppConsole::SetAppName( wxT( "castor_viewer" ) );
		wxAppConsole::SetVendorName( wxT( "dragonjoker" ) );

#if wxCHECK_VERSION( 2, 9, 0 )

		wxAppConsole::SetAppDisplayName( m_displayName );
		wxAppConsole::SetVendorDisplayName( wxT( "DragonJoker" ) );

#endif

		m_mainFrame = new MainFrame{ GuiCommon::make_wxString( m_displayName ) };
		bool result = m_mainFrame->initialise( splashScreen );

		if ( result )
		{
			if ( !getFileName().empty() )
			{
				m_mainFrame->loadScene( getFileName() );
			}
		}
		else
		{
			delete m_mainFrame;
			m_mainFrame = nullptr;
		}

		return m_mainFrame;
	}
}
