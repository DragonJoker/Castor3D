#include "CastorViewer.hpp"
#include "MainFrame.hpp"

#include <wx/cmdline.h>

#include <Pass.hpp>
#include <Submesh.hpp>
#include <TextureUnit.hpp>

#include <File.hpp>
#include <Exception.hpp>

#include <ImagesLoader.hpp>
#include <RendererSelector.hpp>

#include <xpms/castor_transparent.xpm>
#include <xpms/export.xpm>
#include <xpms/log.xpm>
#include <xpms/mat_blanc.xpm>
#include <xpms/properties.xpm>
#include <xpms/scene_blanc.xpm>

using namespace Castor;
using namespace Castor3D;
using namespace GuiCommon;

#if CHECK_MEMORYLEAKS && defined( VLD_AVAILABLE ) && USE_VLD
#	include <vld.h>
#endif

wxIMPLEMENT_APP( CastorViewer::CastorViewerApp );

namespace CastorViewer
{
	CastorViewerApp::CastorViewerApp()
		: CastorApplication( cuT( "CastorViewer" ), cuT( "Castor Viewer" ), 7 )
		, m_mainFrame( NULL )
	{
	}

	void CastorViewerApp::DoLoadAppImages()
	{
		ImagesLoader::AddBitmap( CV_IMG_CASTOR, castor_transparent_xpm );
		ImagesLoader::AddBitmap( eBMP_SCENES, scene_blanc_xpm );
		ImagesLoader::AddBitmap( eBMP_MATERIALS, mat_blanc_xpm );
		ImagesLoader::AddBitmap( eBMP_EXPORT, export_xpm );
		ImagesLoader::AddBitmap( eBMP_LOGS, log_xpm );
		ImagesLoader::AddBitmap( eBMP_PROPERTIES, properties_xpm );
	}

	wxWindow * CastorViewerApp::DoInitialiseMainFrame( GuiCommon::SplashScreen * p_splashScreen )
	{
		wxAppConsole::SetAppName( wxT( "castor_viewer" ) );
		wxAppConsole::SetVendorName( wxT( "dragonjoker" ) );

#if wxCHECK_VERSION( 2, 9, 0 )

		wxAppConsole::SetAppDisplayName( m_displayName );
		wxAppConsole::SetVendorDisplayName( wxT( "DragonJoker" ) );

#endif

		m_mainFrame = new MainFrame( p_splashScreen, make_wxString( m_internalName ) );
		m_mainFrame->Maximize();
		bool l_return = m_mainFrame->Initialise();

		if ( l_return )
		{
			SetTopWindow( m_mainFrame );

			if ( !GetFileName().empty() )
			{
				m_mainFrame->LoadScene( GetFileName() );
			}
		}
		else
		{
			delete m_mainFrame;
			m_mainFrame = NULL;
		}

		return m_mainFrame;
	}
}
