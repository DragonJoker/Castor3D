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
#include <GuiCommon/xpms/properties.xpm>
#include <GuiCommon/xpms/scene_blanc.xpm>

using namespace castor;
using namespace castor3d;
using namespace GuiCommon;

#if CHECK_MEMORYLEAKS && defined( VLD_AVAILABLE ) && USE_VLD
#	include <vld.h>
#endif

wxIMPLEMENT_APP( CastorViewer::CastorViewerApp );

namespace CastorViewer
{
	CastorViewerApp::CastorViewerApp()
		: CastorApplication{ cuT( "CastorViewer" )
			, cuT( "Castor Viewer" )
			, 7
			, Version{ CastorViewer_VERSION_MAJOR, CastorViewer_VERSION_MINOR, CastorViewer_VERSION_BUILD } }
		, m_mainFrame( nullptr )
	{
	}

	void CastorViewerApp::doLoadAppImages()
	{
		ImagesLoader::addBitmap( eBMP_SCENES, scene_blanc_xpm );
		ImagesLoader::addBitmap( eBMP_MATERIALS, mat_blanc_xpm );
		ImagesLoader::addBitmap( eBMP_EXPORT, export_xpm );
		ImagesLoader::addBitmap( eBMP_LOGS, log_xpm );
		ImagesLoader::addBitmap( eBMP_PROPERTIES, properties_xpm );
	}

	wxWindow * CastorViewerApp::doInitialiseMainFrame( GuiCommon::SplashScreen * p_splashScreen )
	{
		wxAppConsole::SetAppName( wxT( "castor_viewer" ) );
		wxAppConsole::SetVendorName( wxT( "dragonjoker" ) );

#if wxCHECK_VERSION( 2, 9, 0 )

		wxAppConsole::SetAppDisplayName( m_displayName );
		wxAppConsole::SetVendorDisplayName( wxT( "DragonJoker" ) );

#endif

		m_mainFrame = new MainFrame( p_splashScreen, make_wxString( m_displayName ) );
		m_mainFrame->Maximize();
		bool result = m_mainFrame->initialise();

		if ( result )
		{
			SetTopWindow( m_mainFrame );

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
