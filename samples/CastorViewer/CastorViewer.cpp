#include "CastorViewer/CastorViewer.hpp"
#include "CastorViewer/MainFrame.hpp"

#include <wx/cmdline.h>

#include <CastorUtils/Data/File.hpp>
#include <CastorUtils/Exception/Exception.hpp>

#include <GuiCommon/ImagesLoader.hpp>
#include <GuiCommon/RendererSelector.hpp>

#include <GuiCommon/xpms/export.xpm>
#include <GuiCommon/xpms/log.xpm>
#include <GuiCommon/xpms/mat_blanc.xpm>
#include <GuiCommon/xpms/print_screen.xpm>
#include <GuiCommon/xpms/properties.xpm>
#include <GuiCommon/xpms/scene_blanc.xpm>

using namespace GuiCommon;

wxIMPLEMENT_APP( CastorViewer::CastorViewerApp );

namespace CastorViewer
{
	castor3d::Version getVersion()
	{
		return castor3d::Version
		{
			CastorViewer_VERSION_MAJOR,
			CastorViewer_VERSION_MINOR,
			CastorViewer_VERSION_BUILD
		};
	}

	CastorViewerApp::CastorViewerApp()
		: CastorApplication{ cuT( "CastorViewer" )
			, cuT( "Castor Viewer" )
			, 7
			, CastorViewer::getVersion() }
		, m_mainFrame( nullptr )
	{
		wxSetAssertHandler( &CastorApplication::assertHandler );
	}

	void CastorViewerApp::doLoadAppImages()
	{
		ImagesLoader::addBitmap( eBMP_SCENES, scene_blanc_xpm );
		ImagesLoader::addBitmap( eBMP_MATERIALS, mat_blanc_xpm );
		ImagesLoader::addBitmap( eBMP_EXPORT, export_xpm );
		ImagesLoader::addBitmap( eBMP_LOGS, log_xpm );
		ImagesLoader::addBitmap( eBMP_PROPERTIES, properties_xpm );
		ImagesLoader::addBitmap( eBMP_PRINTSCREEN, print_screen_xpm );
	}

	wxWindow * CastorViewerApp::doInitialiseMainFrame( GuiCommon::SplashScreen & splashScreen )
	{
		wxAppConsole::SetAppName( wxT( "castor_viewer" ) );
		wxAppConsole::SetVendorName( wxT( "dragonjoker" ) );

#if wxCHECK_VERSION( 2, 9, 0 )

		wxAppConsole::SetAppDisplayName( m_displayName );
		wxAppConsole::SetVendorDisplayName( wxT( "DragonJoker" ) );

#endif

		m_mainFrame = new MainFrame{ make_wxString( m_displayName ) };
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
